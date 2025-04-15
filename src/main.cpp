#include <GL/glew.h>

#include "appstate.hpp"
#include "camera/camera.hpp"
#include "camera/orthographic.hpp"
#include "camera/perspective.hpp"
#include "camera/projection.hpp"
#include "draw/draw.hpp"
#include "gl/draw_enums.hpp"
#include "gl/framebuffer_enums.hpp"
#include "gl/program.hpp"
#include "gl/texture.hpp"
#include "gl/framebuffer.hpp"
#include "gl/texture_enums.hpp"
#include "gl/synchronization.hpp"
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
#include <barrier>
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

#include <cppostream/glm/glm.hpp>


camera_t camera{nullptr};
plane_t*  p_plane{nullptr};

gl::program basic{nullptr};
gl::program terrain{nullptr};
//gl::program passthru{nullptr};
//gl_mesh<HAS_VERTICIES, STORES_VERTEX_COUNT>*     p_plane;


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

    std::cout << "Loading...\n";
    std::cout << "Loading...\n";
    basic = shader::load("ass/shaders/basic");
    terrain = shader::load("ass/shaders/terrain");
    //passthru = shader::load("ass/shaders/passthru");
    std::cout << "Loaded\n";

    p_plane = new plane_t;
    p_plane->create(glm::vec3{0}, glm::vec2{32}, glm::uvec2{32});

    camera.create(transform{{0, 0, 1}, glm::quat{1, 0, 0, 0}, {1,1,1}}, projection{perspective::make_default()});

    tex0.create(gl::enums::texture::Texture2D, glm::uvec2{1<<10}, gl::enums::texture::format_storage::STORAGE_R8, 1);

    noise::perlin_t::refresh_shader();
    noise::perlin_t::use();
    noise::perlin_t::set_seed(0);
    tex0.bind_base(0, GL_WRITE_ONLY);
    noise::perlin_t::dispatch(glm::uvec2{tex0.texture_size()});

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
    const float fast = 5 * dt;
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


sdl_ext SDL_AppResult SDL_AppIterate(void *appstate)try{
    appstate_t& state = *reinterpret_cast<appstate_t*>(appstate);
    state.time.update();
    refresh_cursor(state);
    update_camera();

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    
    glEnable(GL_DEPTH_TEST);
    terrain.use();
    camera.bind();
    tex0.bind(0);
    gl::draw_indecies(*p_plane);
    

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