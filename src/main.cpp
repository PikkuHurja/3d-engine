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
    uint             chunck_size    = 0;
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

    void create(uint cs, uint cc = 9){
        vao.create();
        vao.bind();
        chunck_count = cc;
        chunck_size = cs;



        ibo.create();
        ibo.bind(gl::enums::buffer::ELEMENT_ARRAY_BUFFER);
        uint subcs = cs-1;
        indecie_count = subcs*subcs*6;
        std::unique_ptr<uint[]> indecie_data{new uint[indecie_count]()};

        size_t index = 0;
        for(uint y = 0; y < subcs; y++){
            for(uint x = 0; x < subcs; x++){
                uint    top_l = y*cs+x, 
                        top_r = top_l+1, 
                        bottom_l = top_l+cs, //(y + 1) * vertex_count.x + x;
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

        count_array.reset(new GLsizei[cc]);
        indecies_array.reset(new size_t[cc]);
        indecie_begin_array.reset(new GLint[cc]);
        for(size_t i = 0; i < cc; i++){

            count_array[i] = indecie_count;
            indecies_array[i] = 0;
            indecie_begin_array[i]=i*cs*cs;

        }

        ibo.data(indecie_data.get(), indecie_count*sizeof(uint), gl::enums::buffer::STATIC_DRAW);

        vbo.create();
        vbo.bind(gl::enums::buffer::type::ARRAY_BUFFER);
        vbo.data(nullptr, sizeof(data)*cs*cs*chunck_count, gl::enums::buffer::STATIC_DRAW);

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

    void gen(uint index, glm::ivec2 chunck_position, gl::texture* noise = nullptr){
        if(!sh::_S_Program) sh::refresh_shader();
        sh::use();
        sh::set_access_offset(index);
        sh::set_chunck_position(chunck_position);
        sh::set_chunck_size(glm::uvec2{chunck_size});
        sh::set_height_map_strenght(99);

        vbo.bind(gl::enums::buffer::SHADER_STORAGE_BUFFER);
        vbo.bind_base(gl::enums::buffer::SHADER_STORAGE_BUFFER, 0);

        if(noise)
            noise->bind(0);
        
        sh::dispatch(glm::uvec2{chunck_size});
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
        glMultiDrawElementsBaseVertex(GL_TRIANGLES, count_array.get(), GL_UNSIGNED_INT, reinterpret_cast<const void**>(indecies_array.get()), chunck_count, indecie_begin_array.get());
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
gl::framebuffer fb0{nullptr};
gl::framebuffer fb1{nullptr};
int frequency = 1;
int seed = 1;

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
    //terrain = shader::load("ass/shaders/terrain");
    //passthru = shader::load("ass/shaders/passthru");

    a_plane[0] = new plane_t(glm::vec3{0}, glm::vec2{1}, glm::uvec2{16});
    //a_plane[1] = new plane_t(glm::vec3{0}, glm::vec2{1}, glm::uvec2{1<<9});
    //a_plane[2] = new plane_t(glm::vec3{0}, glm::vec2{1}, glm::uvec2{1<<8});
    //a_plane[3] = new plane_t(glm::vec3{0}, glm::vec2{1}, glm::uvec2{1<<7});
    


    camera.create(transform{{0, 0, 1}, glm::quat{1, 0, 0, 0}, {1,1,1}}, projection{perspective::make_default()});

    fb0.create();
    tex0.create(gl::enums::texture::Texture2D, glm::uvec2{1<<8}, gl::enums::texture::format_storage::STORAGE_R8, 1);
    tex0.parameter(gl::enums::texture::parameter::TEXTURE_MIN_FILTER, GL_LINEAR);
    tex0.parameter(gl::enums::texture::parameter::TEXTURE_MAG_FILTER, GL_LINEAR);
    fb0.attach(tex0, gl::enums::framebuffer::COLOR_ATTACHMENT0);

    noise::perlin_t::refresh_shader();
    noise::perlin_t::use();
    noise::perlin_t::set_frequency(0.25);
    noise::perlin_t::set_seed(0);
    tex0.bind_base(0, GL_WRITE_ONLY);
    noise::perlin_t::dispatch(glm::uvec2{tex0.texture_size()});
    gl::barrier(gl::enums::barriers::SHADER_IMAGE_ACCESS);


    shader::terrain_gen_t::refresh_shader();
    terr.create(64);

    //fb1.create();
    //tex1.create(gl::enums::texture::Texture2D, glm::uvec2{1<<6}, gl::enums::texture::format_storage::STORAGE_R8, 1);
    //fb1.attach(tex1, gl::enums::framebuffer::COLOR_ATTACHMENT0);
    //fb0.blit(fb1, glm::ivec2{0}, glm::ivec2{tex0.texture_size()}, glm::ivec2{0}, glm::ivec2{tex1.texture_size()});

    //capture_cursor(state, true);
    return SDL_APP_CONTINUE;
}catch(const std::exception& e){
    std::cerr << "Uncaught exception at SDL_AppInit: " << e.what() << '\n'; 
    return SDL_APP_FAILURE;
}


void update_camera(){
    const bool* keystate = SDL_GetKeyboardState(nullptr);
    const float dt = appstate_t::_S_ActiveState->time.delta_timef();
    const float slow = 1 * dt;
    const float fast = 20 * dt;
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


    noise::perlin_t::use();
    noise::perlin_t::set_frequency(0.1f);
    noise::perlin_t::set_seed(0);
    tex0.bind_base(0, GL_WRITE_ONLY);
    noise::perlin_t::dispatch(glm::uvec2{tex0.texture_size()});
    gl::barrier(gl::enums::barriers::SHADER_IMAGE_ACCESS);

    for(size_t i = 0; i < terr.chunck_count; i++){
        terr.gen(i, glm::ivec2{i, 0}, &tex0);
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    
    glEnable(GL_DEPTH_TEST);

    
    //terrain.use();
    //camera.bind();
    //tex0.bind(0);
    basic.use();
    terr.draw_all();

    terr.vao.bind();
    glPointSize(3.2);
    glDrawArrays(GL_POINTS, 0, terr.chunck_size*terr.chunck_size);
    //glDrawArrays(GL_LINE_STRIP, 0, terr.chunck_size*terr.chunck_size);
    //glDrawArrays(GL_TRIANGLES, 0, terr.chunck_size*terr.chunck_size);

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
            lod = std::clamp(lod+1, 0, 3);
            std::cout << "lod: " << lod << '\n';
        }else if(event->key.key == SDLK_DOWN){
            lod = std::clamp(lod-1, 0, 3);
            std::cout << "lod: " << lod << '\n';
        }else if(event->key.key == SDLK_R){
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