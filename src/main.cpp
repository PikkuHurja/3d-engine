#include <GL/glew.h>

#include "appstate.hpp"
#include "gl/framebuffer_enums.hpp"
#include "gl/texture.hpp"
#include "gl/framebuffer.hpp"
#include "gl/texture_enums.hpp"
#include <exception>
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



gl::texture tex;
gl::framebuffer fb{nullptr};
int frequency = 1;
int seed = 1;

sdl_ext SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv)try{
    appstate_t::_S_ActiveState = *reinterpret_cast<appstate_t**>(appstate) = new appstate_t;
    appstate_t& state = *appstate_t::_S_ActiveState;
    state.init();
    noise::perlin_t::refresh_shader();
    tex.create(gl::enums::texture::Texture2D, glm::uvec2{1<<12}, gl::enums::texture::format_storage::STORAGE_R8, 1);
    fb.create();
    fb.attach(tex, gl::enums::framebuffer::COLOR_ATTACHMENT0);
    return SDL_APP_CONTINUE;
}catch(const std::exception& e){
    std::cerr << "Uncaught exception at SDL_AppInit: " << e.what() << '\n'; 
    return SDL_APP_FAILURE;
}



sdl_ext SDL_AppResult SDL_AppIterate(void *appstate)try{
    appstate_t& state = *reinterpret_cast<appstate_t*>(appstate);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    noise::perlin_t::use();
    //noise::perlin_t::set_octave_count();
    noise::perlin_t::set_frequency(frequency);
    noise::perlin_t::set_seed(seed);
    tex.bind_base(0, GL_WRITE_ONLY);
    noise::perlin_t::dispatch(glm::uvec2{tex.texture_size()});

    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    fb.blit_screen(glm::ivec2{0}, tex.texture_size(), glm::ivec2{0}, state.core.p_window->GetWindowSize(), gl::enums::framebuffer::mask::COLOR, gl::enums::framebuffer::LINEAR);

    SDL::GL::SwapWindow(*state.core.p_window);
    return SDL_APP_CONTINUE;
}catch(const std::exception& e){
    std::cerr << "Uncaught exception at SDL_AppIterate: " << e.what() << '\n'; 
    return SDL_APP_FAILURE;
}


sdl_ext SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)try{
    //appstate_t& state = *reinterpret_cast<appstate_t*>(appstate);
    if(event->type == SDL_EVENT_QUIT)
        return SDL_APP_SUCCESS;


    if(event->type == SDL_EVENT_KEY_DOWN){
        if(event->key.key == SDLK_W){
            frequency++;
            std::cout << "frequency: " << frequency << '\n';
        }else if(event->key.key == SDLK_S){
            frequency--;
            std::cout << "frequency: " << frequency << '\n';
        }

        if(event->key.key == SDLK_D){
            seed++;
            std::cout << "seed: " << seed << '\n';
        }else if(event->key.key == SDLK_A){
            seed--;
            std::cout << "seed: " << seed << '\n';
        }
    }


    return SDL_APP_CONTINUE;
}catch(const std::exception& e){
    std::cerr << "Uncaught exception at SDL_AppEvent: " << e.what() << '\n'; 
    return SDL_APP_FAILURE;
}


sdl_ext void SDL_AppQuit(void *appstate, SDL_AppResult result)try{
    delete reinterpret_cast<appstate_t*>(appstate);
    appstate_t::_S_ActiveState = nullptr;

}catch(const std::exception& e){
    std::cerr << "Uncaught exception at SDL_AppQuit: " << e.what() << '\n'; 
    return;
}