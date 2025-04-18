#include <GL/glew.h>

#include "appstate.hpp"
#include "camera/camera.hpp"
#include "camera/orthographic.hpp"
#include "camera/perspective.hpp"
#include "camera/projection.hpp"
#include "draw/draw.hpp"
#include "gl/buffer.hpp"
#include "gl/buffer_enums.hpp"
#include "gl/draw_enums.hpp"
#include "gl/framebuffer_enums.hpp"
#include "gl/program.hpp"
#include "gl/shader_spec.hpp"
#include "gl/texture.hpp"
#include "gl/framebuffer.hpp"
#include "gl/texture_enums.hpp"
#include "gl/synchronization.hpp"
#include "gl/vertex_array.hpp"
#include "gl_mesh.hpp"
#include "obj/plane.hpp"
#include "obj/transform.hpp"
#include "shader/load.hpp"
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_keyboard.h>
#include <SDL3/SDL_keycode.h>
#include <SDL3/SDL_mouse.h>
#include <SDL3/SDL_oldnames.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_scancode.h>
#include <algorithm>
#include <barrier>
#include <cassert>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <exception>
#include <glm/ext/quaternion_float.hpp>
#include <glm/ext/quaternion_geometric.hpp>
#include <glm/ext/quaternion_trigonometric.hpp>
#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/ext/vector_float4.hpp>
#include <glm/ext/vector_int2.hpp>
#include <glm/ext/vector_uint2.hpp>
#include <iostream>
#include <memory>
#include <sys/types.h>
#define SDL_MAIN_USE_CALLBACKS
#define sdl_ext extern "C" 
#include <SDL3/SDL_main.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_video.h>
#include <SDL3/SDL_events.h>

#include <SDL/SDL.h>
#include <SDL/Window.h>
#include <SDL/GL.h>

#include "noise/perlin.hpp"
#include "noise/perlin-tileable.hpp"
#include "noise/value.hpp"
#include "shaders/terrain-gen.hpp"

#include <cppostream/glm/glm.hpp>


struct terrain{
    using sh = shader::terrain_gen_t;
    gl::vertex_array vao{nullptr};
    gl::basic_buffer vbo{nullptr};
    gl::basic_buffer ibo{nullptr}; // index buffer
    float            chunck_size    = 0;
    uint             chunck_side_vertex_count    = 0;
    uint             chunck_count   = 0;
    uint             indecie_count  = 0;

    std::unique_ptr<GLsizei[]>  count_array; // the same values [indecie_count], repeated
    std::unique_ptr<size_t[]>  indecies_array; // the same values [0], repeated
    std::unique_ptr<GLint[]>    indecie_begin_array; // [n*chunck_size.x*chunck_size.y], n in +integer

    struct data{
        glm::vec3 vertex;
        glm::vec2 uv;
        glm::vec3 n, t, bt;
    };

    void create(uint vtx, float cs, uint cc = 9){
        vao.create();
        vao.bind();
        chunck_count = cc;
        chunck_size = cs;
        chunck_side_vertex_count = vtx;



        ibo.create();
        ibo.bind(gl::enums::buffer::ELEMENT_ARRAY_BUFFER);
        uint sub_vtx = vtx-1;
        indecie_count = sub_vtx*sub_vtx*6;
        std::unique_ptr<uint[]> indecie_data{new uint[indecie_count]()};

        size_t index = 0;
        for(uint y = 0; y < sub_vtx; y++){
            for(uint x = 0; x < sub_vtx; x++){
                uint    top_l = y*vtx+x, 
                        top_r = top_l+1, 
                        bottom_l = top_l+vtx, //(y + 1) * vertex_count.x + x;
                        bottom_r = bottom_l+1;


                if(index+6 > indecie_count)
                    std::cerr << index+6 << " > " << indecie_count << '\n';

                indecie_data[index++] = top_l;
                indecie_data[index++] = bottom_l;
                indecie_data[index++] = top_r;

                indecie_data[index++] = top_r;
                indecie_data[index++] = bottom_l;
                indecie_data[index++] = bottom_r;
            }
        }

        count_array.reset(new GLsizei[chunck_count]);
        indecies_array.reset(new size_t[chunck_count]);
        indecie_begin_array.reset(new GLint[chunck_count]);
        for(size_t i = 0; i < chunck_count; i++){

            count_array[i] = indecie_count;
            indecies_array[i] = 0;
            indecie_begin_array[i]=i*vtx*vtx;

        }

        ibo.data(indecie_data.get(), indecie_count*sizeof(uint), gl::enums::buffer::STATIC_DRAW);

        vbo.create();
        vbo.bind(gl::enums::buffer::type::ARRAY_BUFFER);
        vbo.data(nullptr, sizeof(data)*vtx*vtx*chunck_count, gl::enums::buffer::STATIC_DRAW);

        vbo.attribute(gl::shader_spec::aVertex, 3, gl::enums::buffer::FLOAT, sizeof(data), false, 0);
        vbo.attribute(gl::shader_spec::aUV, 2, gl::enums::buffer::FLOAT, sizeof(data), false, sizeof(float)*(3));
        vbo.attribute(gl::shader_spec::aNormal, 3, gl::enums::buffer::FLOAT, sizeof(data), false, sizeof(float)*(3+2));
        vbo.attribute(gl::shader_spec::aTangent, 3, gl::enums::buffer::FLOAT, sizeof(data), false, sizeof(float)*(3+2+3));
        vbo.attribute(gl::shader_spec::aBitangent, 3, gl::enums::buffer::FLOAT, sizeof(data), false, sizeof(float)*(3+2+3+3));

        vao.enable_attribute(gl::shader_spec::aVertex);
        vao.enable_attribute(gl::shader_spec::aUV);
        vao.enable_attribute(gl::shader_spec::aNormal);
        vao.enable_attribute(gl::shader_spec::aTangent);
        vao.enable_attribute(gl::shader_spec::aBitangent);
        vao.unbind();
        vbo.unbind(gl::enums::buffer::ARRAY_BUFFER);
        ibo.unbind(gl::enums::buffer::ELEMENT_ARRAY_BUFFER);
    }

    void gen(
        const uint       &index, 
        const glm::ivec2 &chunck_position, 
        //const uint       &vertex_count,  //stored in the class
        //const float      &chunck_size, //stored in the class
        const float      &height_map_strenght = 1, 
        const uint       &noise_seed = 0, 
        const float      &noise_frequency = .2, 
        const int        &noise_octave_count = 12, 
        const float      &noise_persistence = 0.5, 
        const float      &noise_lacunarity = 2.0
    ){
        if(!sh::_S_Program) sh::refresh_shader();
        sh::use(
            index, 
            chunck_position, 
            chunck_side_vertex_count,
            chunck_size, 
            height_map_strenght, 
            noise_seed, 
            noise_frequency, 
            noise_octave_count, 
            noise_persistence, 
            noise_lacunarity
        );

        vbo.bind(gl::enums::buffer::SHADER_STORAGE_BUFFER);
        vbo.bind_base(gl::enums::buffer::SHADER_STORAGE_BUFFER, 0);
        sh::dispatch(glm::uvec2{chunck_side_vertex_count});
    }

    void draw(){
        vao.bind();
        //glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
        gl::draw_indecies(gl::enums::TRIANGLES, indecie_count);
        //glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
        //gl::draw_indecies(gl::enums::TRIANGLES, indecie_count);
    }
    
    void draw_all(){
        vao.bind();
        //glPolygonMode( GL_FRONT_AND_BACK, GL_POINT );
        glMultiDrawElementsBaseVertex(GL_TRIANGLES, count_array.get(), GL_UNSIGNED_INT, reinterpret_cast<const void**>(indecies_array.get()), chunck_count, indecie_begin_array.get());
        //glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
    }

} terr;

camera_t camera{nullptr};
/*
gl::vertex_array vao{nullptr};
gl::typed_buffer_t<gl::enums::buffer::ARRAY_BUFFER> vbo{nullptr};
size_t vertex_count = 0;

    gl::program terrain_tess{nullptr};
    terrain_tess = shader::load("ass/shaders/terrain-tess");
    vao.create();
    vao.bind();
    vbo.create();
    {
        std::vector<glm::vec3> verts;
        int gridSize = 64;

        for (int z = 0; z <= gridSize; ++z) {
            for (int x = 0; x <= gridSize; ++x) {
                float x0 = ((float)x     / gridSize);
                float x1 = ((float)(x+1) / gridSize);
                float z0 = ((float)z     / gridSize);
                float z1 = ((float)(z+1) / gridSize);

                // Define 1 quad (patch of 4 vertices)
                verts.push_back(glm::vec3(x0, 0.f, z0)); // bottom-left
                verts.push_back(glm::vec3(x1, 0.f, z0)); // bottom-right
                verts.push_back(glm::vec3(x1, 0.f, z1)); // top-right
                verts.push_back(glm::vec3(x0, 0.f, z1)); // top-left
            }
        }
        vertex_count = verts.size();
        vbo.bind();
        vbo.data(verts.data(), sizeof(glm::vec3) * verts.size(), gl::enums::buffer::STATIC_DRAW);

        vbo.attribute(gl::shader_spec::aVertex, 3, gl::enums::buffer::FLOAT);
        vao.enable_attribute(gl::shader_spec::aVertex);
        vao.unbind();
        vbo.unbind();
    }

    terrain_tess.use();
    camera.bind();
    tex0.bind(0);
    glPatchParameteri(GL_PATCH_VERTICES, 4);
    vao.bind();
    gl::draw_one(gl::enums::PATCHES, 0, vertex_count);

*/

//gl::program terrain{nullptr};

gl::program basic{nullptr};
//gl::program passthru{nullptr};
//gl_mesh<HAS_VERTICIES, STORES_VERTEX_COUNT>*     p_plane;
    //plane levels of detail
plane_t* a_plane[4];
int lod = 0;

float pitch     = 0;
float yaw       = 0;

gl::texture tex0;
gl::texture tex1;
gl::texture a_tex[16];
gl::framebuffer fb0{nullptr};
gl::framebuffer fb1{nullptr};
int frequency = 1;
int seed = 1;

uint vtx_count = 1<<7;

bool should_capture_cursor = false;
void capture_cursor(appstate_t& state, bool on){
    should_capture_cursor = on;
    if(!SDL_SetWindowRelativeMouseMode(*state.core.p_window, on))
        std::cerr << SDL_GetError() << '\n';
    if(!SDL_SetWindowMouseGrab(*state.core.p_window, on))
        std::cerr << SDL_GetError() << '\n';
}
void refresh_cursor(appstate_t& state){
    SDL_SetWindowRelativeMouseMode(*state.core.p_window, should_capture_cursor);
    SDL_SetWindowMouseGrab(*state.core.p_window, should_capture_cursor);
}



sdl_ext SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv)try{
    appstate_t::_S_ActiveState = *reinterpret_cast<appstate_t**>(appstate) = new appstate_t;
    appstate_t& state = *appstate_t::_S_ActiveState;
    state.init();

    basic = shader::load("ass/shaders/basic");

    camera.create(transform{{0, 0, 1}, glm::quat{1, 0, 0, 0}, {1,1,1}}, projection{perspective::make_default()});

    for(auto& e : a_tex){
        e.create(gl::enums::texture::Texture2D, glm::uvec2{1<<2}, gl::enums::texture::format_storage::STORAGE_R8, 1);
        e.parameter(gl::enums::texture::parameter::TEXTURE_MIN_FILTER, GL_LINEAR);
        e.parameter(gl::enums::texture::parameter::TEXTURE_MAG_FILTER, GL_LINEAR);
    }


    shader::terrain_gen_t::refresh_shader();
    terr.create(vtx_count,1<<10);

    return SDL_APP_CONTINUE;
}catch(const std::exception& e){
    std::cerr << "Uncaught exception at SDL_AppInit: " << e.what() << '\n'; 
    return SDL_APP_FAILURE;
}


void update_camera(){
    const bool* keystate = SDL_GetKeyboardState(nullptr);
    const float dt = appstate_t::_S_ActiveState->time.delta_timef();
    const float slow = 1 * dt;
    const float fast = 256 * dt;
    float movement = keystate[SDL_SCANCODE_LSHIFT] ? fast : slow;
    if(keystate[SDL_SCANCODE_W]){
        camera.translation() += movement*camera.forward();
    }else if(keystate[SDL_SCANCODE_S]){
        camera.translation() += movement*camera.backward();
    }
    if(keystate[SDL_SCANCODE_D]){
        camera.translation() += movement*camera.rightward();
    }else if(keystate[SDL_SCANCODE_A]){
        camera.translation() += movement*camera.leftward();
    }

    if(keystate[SDL_SCANCODE_SPACE]){
        camera.translation() += movement*camera.upward();
    }else if(keystate[SDL_SCANCODE_C]){
        camera.translation() += movement*camera.downward();
    }
    camera.v_rotation = glm::quat{glm::vec3{pitch, yaw, 0.f}};
    camera.refresh();
}

uint i = 0;
sdl_ext SDL_AppResult SDL_AppIterate(void *appstate)try{
    appstate_t& state = *reinterpret_cast<appstate_t*>(appstate);
    state.time.update();
    refresh_cursor(state);
    update_camera();

    size_t index = 0;
    for(int y = 0; y <= 2; y++){
        for(int x = 0; x <= 2; x++){
            terr.gen(index++, glm::ivec2{x, y}, (1<<9)/2, seed);
        }
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    basic.use();
    camera.bind();
    terr.draw_all();

    SDL::GL::SwapWindow(*state.core.p_window);
    return SDL_APP_CONTINUE;
}catch(const std::exception& e){
    std::cerr << "Uncaught exception at SDL_AppIterate: " << e.what() << '\n'; 
    return SDL_APP_FAILURE;
}


sdl_ext SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)try{
    appstate_t& state = *reinterpret_cast<appstate_t*>(appstate);
    if(event->type == SDL_EVENT_QUIT){
        capture_cursor(state, false);   //hangs for a while if set to true
        return SDL_APP_SUCCESS;
    }

    float sens = -0.001;

        //rotate camere
    if(event->type == SDL_EVENT_MOUSE_MOTION){
        yaw+=event->motion.xrel*sens;
        pitch+=event->motion.yrel*sens;
    }
        //lock cursor
    if(event->type == SDL_EVENT_KEY_DOWN){
        if(event->key.key == SDLK_ESCAPE){
            capture_cursor(state, false);
        }else if(event->key.key == SDLK_L){
            capture_cursor(state, true);
        }else if(event->key.key == SDLK_UP){
            vtx_count--;
            std::cout << "vtx_count: " << vtx_count << '\n';
            terr.create(vtx_count, 1<<10);
        }else if(event->key.key == SDLK_DOWN){
            vtx_count++;
            std::cout << "vtx_count: " << vtx_count << '\n';
            terr.create(vtx_count, 1<<10);
        }else if(event->key.key == SDLK_R){
            seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
            //terrain = shader::load("ass/shaders/terrain");
        }
    }



    return SDL_APP_CONTINUE;
}catch(const std::exception& e){
    std::cerr << "Uncaught exception at SDL_AppEvent: " << e.what() << '\n'; 
    return SDL_APP_FAILURE;
}


sdl_ext void SDL_AppQuit(void *appstate, SDL_AppResult result)try{
    std::cout << "Quitting\n";
    delete reinterpret_cast<appstate_t*>(appstate);
    appstate_t::_S_ActiveState = nullptr;

}catch(const std::exception& e){
    std::cerr << "Uncaught exception at SDL_AppQuit: " << e.what() << '\n'; 
    return;
}