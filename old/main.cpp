#include <GL/glew.h>

#include "appstate.hpp"
#include "camera/camera.hpp"
#include "camera/perspective.hpp"
#include "camera/projection.hpp"
#include "cubemap/cube-map.hpp"
#include "cubemap/shadow-map.hpp"
#include "cubemap/skybox.hpp"
#include "draw/draw.hpp"
#include "extensions.hpp"
#include "gl/buffer.hpp"
#include "gl/buffer_enums.hpp"
#include "gl/draw_enums.hpp"
#include "gl/framebuffer_enums.hpp"
#include "gl/program.hpp"
#include "gl/shader_spec.hpp"
#include "gl/texture.hpp"
#include "gl/framebuffer.hpp"
#include "gl/texture_enums.hpp"
#include "gl/vertex_array.hpp"
#include "obj/transform.hpp"
#include "shader/load.hpp"
#include <SDL3/SDL_error.h>
#include <SDL3/SDL_keyboard.h>
#include <SDL3/SDL_keycode.h>
#include <SDL3/SDL_mouse.h>
#include <SDL3/SDL_oldnames.h>
#include <SDL3/SDL_pixels.h>
#include <SDL3/SDL_scancode.h>
#include <cassert>
#include <chrono>
#include <cstddef>
#include <cstdlib>
#include <exception>
#include <glm/ext/matrix_float3x3.hpp>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/quaternion_float.hpp>
#include <glm/ext/quaternion_geometric.hpp>
#include <glm/ext/quaternion_trigonometric.hpp>
#include <glm/ext/scalar_constants.hpp>
#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/ext/vector_float4.hpp>
#include <glm/ext/vector_int2.hpp>
#include <glm/ext/vector_uint2.hpp>
#include <glm/matrix.hpp>
#include <iostream>
#include <memory>
#include <ostream>
#include <span>
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

#include "shaders/terrain-gen.hpp"

#include <cppostream/glm/glm.hpp>



void clear_error(){
    while (glGetError()) {}
}

void check_error(const char* msg){
    while (int err = glGetError()) {
        std::cerr << msg << ": " << err << '\n';
    }
}

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

};

void terrain_gen(terrain& terr, uint seed){
    size_t index = 0;
    for(int y = 0; y <= 2; y++){
        for(int x = 0; x <= 2; x++){
            terr.gen(index++, glm::ivec2{x, y}, (1<<9)/2, seed);
        }
    }
}



struct rect2D{
    glm::vec2 min;
    glm::vec2 max;
};
struct rect3D{
    glm::vec3 min;
    glm::vec3 left;
    glm::vec3 right;

    inline void verticies(glm::vec3 vert[4])const{
        vert[0] = min;
        vert[1] = left;
        vert[2] = left + right - min;
        vert[3] = right;
    }
};
    //i love to make everything so complicated
    //instanced, ima kms ig
    //nvm need an instancer
struct rectangle{

    struct in_gpu{
        glm::vec3 verticies[4];
        glm::vec2 uv[4] = {{0, 0}, {0, 1}, {1, 1}, {1, 0}};
        /*
            norm, binorm etc can be differentiated some other time
        */
        in_gpu(){}
        in_gpu(const rect3D& r){r.verticies(verticies);}
    };

    
    rect3D v_rect; // actual data

    gl::vertex_array gl_vao{nullptr};
    gl::basic_buffer gl_vbo{nullptr};
    
    inline void set(const rect3D& r){
        v_rect = r;
    }

    inline void upload(){
        glm::vec3 verticies[4];
        v_rect.verticies(verticies);
        gl_vbo.subdata(verticies, sizeof(verticies));
    }

    inline void create(const rect3D& r){
        set(r);

        if(!gl_vao)
            gl_vao.create();
        if(!gl_vbo){
            gl_vbo.create();
            create_storage();
        }else{
            upload();
        }
    }
        //setup the model matricies beforehand 
    inline void draw(){
        gl_vao.bind();
        gl::draw_one(gl::enums::drawmode::TRIANGLE_FAN, 0, 4);
    }

    inline rectangle(std::nullptr_t = nullptr){}
    inline rectangle(const rect3D& r):v_rect(r), gl_vao(), gl_vbo(){
        create_storage();
    }

protected:

    inline void setup_attributes(){
        gl_vao.bind();
        gl_vbo.bind(gl::enums::buffer::ARRAY_BUFFER);
        
        gl_vao.enable_attribute(gl::shader_spec::aVertex);
        gl_vbo.attribute(gl::shader_spec::aVertex, 3, gl::enums::buffer::FLOAT, sizeof(glm::vec3), false, 0);

        gl_vao.enable_attribute(gl::shader_spec::aUV);
        gl_vbo.attribute(gl::shader_spec::aUV, 2, gl::enums::buffer::FLOAT, sizeof(glm::vec2), false, 4 * sizeof(glm::vec3));
        
        gl_vao.unbind();
        gl_vbo.unbind(gl::enums::buffer::ARRAY_BUFFER);
    }

    inline void create_storage(){
        in_gpu data{v_rect};
        gl_vbo.storage(&data, sizeof(data), gl::enums::buffer::DYNAMIC_STORAGE);
        setup_attributes();
    }

};


gl::framebuffer render_target{nullptr};
gl::texture     render_color{nullptr};
gl::texture     render_depth{nullptr};

rectangle front{nullptr};
rectangle back{nullptr};
rectangle left{nullptr};
rectangle right{nullptr};
rectangle top{nullptr};
rectangle bottom{nullptr};

terrain terr;


camera_t camera{nullptr};
camera_t sh_camera{nullptr};

skybox_t skybox{nullptr};

glm::vec3 sh_map_position{250, 30, 200};
gl::framebuffer sh_fb{nullptr};
shadow_map  sh_map{nullptr};
cube_map    cb_map{nullptr};
gl::program basic{nullptr};

gl::program linear_depth{nullptr};
gl::program light_tint{nullptr};
gl::program tint{nullptr};
gl::program white{nullptr};

rectangle glass_panel{nullptr};
rectangle ground_plane{nullptr};

int lod = 0;


float pitch     = 0;
float yaw       = 0;
int frequency = 1;
int seed = 1;
uint vtx_count = 1<<7;

void create_cube(){
    float size = 1;
    glm::vec3 position{sh_map_position};
    front.create(rect3D     {position + glm::vec3{-size, -size,  size},    position + glm::vec3{-size,  size,  size}, position + glm::vec3{ size, -size,  size}});
    back.create(rect3D      {position + glm::vec3{-size, -size, -size},    position + glm::vec3{-size,  size, -size}, position + glm::vec3{ size, -size, -size}});
    left.create(rect3D      {position + glm::vec3{-size, -size, -size},    position + glm::vec3{-size,  size, -size}, position + glm::vec3{-size, -size,  size}});
    right.create(rect3D     {position + glm::vec3{ size, -size, -size},    position + glm::vec3{ size,  size, -size}, position + glm::vec3{ size, -size,  size}});
    top.create(rect3D       {position + glm::vec3{-size,  size, -size},    position + glm::vec3{-size,  size,  size}, position + glm::vec3{ size,  size, -size}});
    bottom.create(rect3D    {position + glm::vec3{-size, -size, -size},    position + glm::vec3{-size, -size,  size}, position + glm::vec3{ size, -size, -size}});
}

void draw_cube(){
    front.draw();
    back.draw();
    left.draw();
    right.draw();
    top.draw();
    bottom.draw();
}


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

void print_extensions(){
    if(!appstate_t::gl_t::s_has_initialized)
        std::cerr << "print_extensions: 'opengl was not initialized!'" << std::endl;
    
    appstate_t::gl_t::after_gl([](){
        GLint n;
        glGetIntegerv(GL_NUM_EXTENSIONS, &n);
        for (int i = 0; i < n; ++i) {
            const char* ext = (const char*)glGetStringi(GL_EXTENSIONS, i);
            std::cout << i << ": '" << ext << "'\n";
        }
    });
}


sdl_ext SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv)try{


    appstate_t::_S_ActiveState = *reinterpret_cast<appstate_t**>(appstate) = new appstate_t;
    appstate_t& state = *appstate_t::_S_ActiveState;
    state.init();

    print_extensions();
    assert(gl::ext::is_supported(gl::ext::blend_equation_advanced));
    assert(gl::ext::is_supported(gl::ext::shader_framebuffer_fetch));

    basic = shader::load("ass/shaders/basic");
    
    linear_depth = shader::load("ass/shaders/linear-depth");
    light_tint = shader::load("ass/shaders/light-tint");

    tint = shader::load("ass/shaders/tint");
    white = shader::load("ass/shaders/white");

    camera.create(transform_t{{-208, 3, 55}, glm::quat{1, 0, 0, 0}, {1,1,1}}, projection_t{perspective_t::make_default()});
    sh_camera.create(transform_t{{0, 0, 1}, glm::quat{1, 0, 0, 0}, {1,1,1}}, projection_t{perspective_t::make_default()});

    glm::uvec2 window_size = state.core.p_window->GetWindowSize();
    render_color.create(gl::enums::texture::Texture2D, window_size, gl::enums::texture::STORAGE_RGB8, 1);
    render_depth.create(gl::enums::texture::Texture2D, window_size, gl::enums::texture::STORAGE_DEPTH_COMPONENT24, 1);
    render_target.create();
    render_target.attach(render_depth, gl::enums::framebuffer::DEPTH_ATTACHMENT);
    render_target.attach(render_color, gl::enums::framebuffer::COLOR_ATTACHMENT0);

    skybox.refresh_shader();
    skybox.create_cube();
    skybox.create(1<<10, gl::enums::texture::STORAGE_RGB8);
    skybox.texture().clear(glm::vec4{.1});

    shader::terrain_gen_t::refresh_shader();
    terr.create(vtx_count,1<<10);
    ground_plane.create(rect3D{{0,0,0}, {-200, 0, 0}, {0, 0, 200}});
    glass_panel.create(rect3D{{25, -25, 255}, {25, 20, 255}, {205, -25, 270}});

    sh_map.create(1<<12);
    cb_map.create(1<<10, gl::enums::texture::STORAGE_RGB8);
    sh_fb.create();

    create_cube();


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

void draw_shadowmap_opaque();
void draw_shadowmap_transparent();

void render_light_depth(glm::mat4* views){
    clear_error();
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);

    linear_depth.use();
    linear_depth.set("light_pos",   sh_map_position);          //vec3
    linear_depth.set("far_z",       cube_map::far_plane);           //float
    linear_depth.set("near_z",      cube_map::near_plane);          //float
    linear_depth.set("shadowmap_matricies", std::span<glm::mat4>{views, 6});        //mat4

    sh_fb.attach(sh_map.texture(), gl::enums::framebuffer::DEPTH_ATTACHMENT, 0);
    sh_fb.bind();
    glClear(GL_DEPTH_BUFFER_BIT);

    terr.draw_all();
    //draw_shadowmap_opaque();
    check_error("render_light_depth");
}
void render_light_tint(glm::mat4* views){
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDepthMask(GL_FALSE);
    
    glEnable(GL_BLEND);
    glBlendEquation(GL_MULTIPLY_KHR);

    light_tint.use();
    light_tint.set("shadowmap_matricies",   std::span<glm::mat4>{views, 6});        //mat4

    //cb_map.texture().clear(glm::vec4(1,1,1,1));


    sh_fb.attach(cb_map.texture(), gl::enums::framebuffer::COLOR_ATTACHMENT0, 0);
    sh_fb.bind();
    glClear(GL_COLOR_BUFFER_BIT);


    draw_shadowmap_transparent();

    sh_fb.detach(cb_map.texture(), gl::enums::framebuffer::COLOR_ATTACHMENT0);
    glDepthMask(GL_TRUE);
    
}


void render_cubemap(appstate_t& state = *appstate_t::_S_ActiveState){
    clear_error();
    glViewport(0, 0, sh_map.texture().texture_size().x, sh_map.texture().texture_size().y);

    glm::mat4 views[6];
    sh_map.views(views, sh_map_position);
    for(auto& e : views){
        e = sh_map.perspective() * e;
    }


    render_light_depth(views);
    render_light_tint(views);
    //FIN

        //reset viewport and framebuffer
    sh_fb.unbind();
    auto wsz= state.core.p_window->GetWindowSize();
    glViewport(0, 0, wsz.x, wsz.y);
    check_error("render_cubemap");
}

/*
void render_cubemap(appstate_t& state = *appstate_t::_S_ActiveState){
    clear_error();
    glViewport(0, 0, sh_map.texture().texture_size().x, sh_map.texture().texture_size().y);

    glm::mat4 views[6];
    sh_map.views(views, sh_map_position);
    for(auto& e : views){
        e = sh_map.perspective() * e;
    }


    linear_depth.use();
    linear_depth.set("light_pos",   sh_map_position);          //vec3
    linear_depth.set("far_z",       cube_map::far_plane);           //float
    linear_depth.set("near_z",      cube_map::near_plane);          //float
    linear_depth.set("shadowmap_matricies", std::span<glm::mat4>{views});        //mat4


    
    //linear_depth.set("model_matrix", views);            //mat4
    sh_fb.attach(sh_map.texture(), gl::enums::framebuffer::DEPTH_ATTACHMENT, 0);

    sh_fb.bind();

    terr.draw_all();


        //reset viewport and framebuffer
    sh_fb.unbind();
    auto wsz= state.core.p_window->GetWindowSize();
    glViewport(0, 0, wsz.x, wsz.y);
    check_error("render_cubemap");
}
*/

void update_cubemap(appstate_t& state = *appstate_t::_S_ActiveState){
    const bool* keystate = SDL_GetKeyboardState(nullptr);
    const float dt = appstate_t::_S_ActiveState->time.delta_timef();
    constexpr float slow = 1;
    constexpr float fast = 64;
    const float movement = (keystate[SDL_SCANCODE_LSHIFT] ? fast : slow) * dt;

    bool updated = true;
    if(keystate[SDL_SCANCODE_UP]){
        sh_map_position += movement*glm::vec3( 1,0, 1);
    }else if(keystate[SDL_SCANCODE_DOWN]){
        sh_map_position += movement*glm::vec3(-1,0,-1);
    }else{
        updated=false;
    }
    if(updated)
        create_cube();

    render_cubemap(state);
}



void draw_shadowmap_opaque(){
    clear_error();
    terr.draw_all();
    check_error("draw_shadowmap_opaque");
}

void draw_shadowmap_transparent(){
    clear_error();
    glass_panel.draw();
    check_error("draw_shadowmap_transparent");
}
void draw_opaque(){
    clear_error();
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);

    basic.use();
    camera.bind();
        
        sh_map.texture().bind(0);
        basic.set("cube_map", 0);
        basic.set("cube_map_position", sh_map_position);
        basic.set("near_z", cube_map::near_plane);
        basic.set("far_z", cube_map::far_plane);

    //ground_plane.draw();
    terr.draw_all();
    check_error("draw_opaque");

}
void draw_skybox(){
    clear_error();
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glDepthMask(GL_FALSE);

        //some weird bullshit
    glm::mat4 model = camera.projection_matrix() * glm::mat4_cast(glm::inverse(camera.v_rotation));
    
    skybox.draw(model);

    glDepthMask(GL_TRUE);
    check_error("draw_skybox");
}
void draw_transparent(){
    clear_error();
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glDepthMask(GL_FALSE);
    glEnable(GL_BLEND);

    glBlendEquation(GL_MULTIPLY_KHR);

    tint.use();
    camera.bind();
    glass_panel.draw();

    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    check_error("draw_transparent");
}



void draw(){
    //render_target.bind();
    gl::framebuffer::unbind();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    draw_opaque();
    draw_skybox();
    draw_transparent();

    //render_target.blit_screen(
    //    {0,0}, 
    //    render_color.texture_size(), 
//
    //    {0,0}, 
    //    appstate_t::_S_ActiveState->core.p_window->GetWindowSize(), 
//
    //    ::gl::enums::framebuffer::mask::COLOR | ::gl::enums::framebuffer::mask::DEPTH, 
    //    gl::enums::framebuffer::filter::NEAREST
    //);
}

uint i = 0;
sdl_ext SDL_AppResult SDL_AppIterate(void *appstate)try{
    appstate_t& state = *reinterpret_cast<appstate_t*>(appstate);
    state.time.update();

    refresh_cursor(state);
    update_camera();
    camera.refresh();
    
    terrain_gen(terr, seed);

    update_cubemap(state);

    draw();

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
        }/*else if(event->key.key == SDLK_UP){
            lod=(lod+1)%6;
            //vtx_count--;
            //std::cout << "vtx_count: " << vtx_count << '\n';
            //terr.create(vtx_count, 1<<10);
        }else if(event->key.key == SDLK_DOWN){
            if(lod <= 0) lod = 5;
            else lod--;
            //vtx_count++;
            //std::cout << "vtx_count: " << vtx_count << '\n';
            //terr.create(vtx_count, 1<<10);
        }*/
        else if(event->key.key == SDLK_R){
            seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
            //terrain = shader::load("ass/shaders/terrain");
        }else if(event->key.key == SDLK_P){
            std::cout << "position" << camera.v_translation << '\n';
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