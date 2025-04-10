#pragma once

#include <SDL/SDL.h>
#include <SDL/Window.h>
#include <opengl-framework/Wrappers/GLEW.hpp>
#include <SDL/GL.h>
#include <functional>
#include <memory>
#include <mutex>
#include <thread>
#include <utility>
#include <vector>



struct appstate_t{
    inline static appstate_t* _S_ActiveState = nullptr;


    struct core_t{
        std::shared_ptr<SDL::SDL3>          p_sdl;
        std::shared_ptr<SDL::Window>        p_window;
        std::shared_ptr<GLEW>               p_glew;
        std::shared_ptr<SDL::GL::Context>   p_gl;

        void init(){
            p_sdl.reset(new SDL::SDL3(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS));

                SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
                SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
                SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
                SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_DEBUG_FLAG);

            p_window.reset(new SDL::Window{"", {1280, 720}, SDL_WINDOW_OPENGL});
            p_gl.reset(new SDL::GL::Context{*p_window});
            glewExperimental = true;
            p_glew.reset(new GLEW());
        }
    } core;


        // currently using only one context, arguable performance gains
        // context is in core
    struct gl_t{
        inline static bool s_has_initialized = false;
        inline static std::vector<std::function<void(void)>> s_init_queue;

        
        inline static void call_queue(){
            s_has_initialized  = true;
            for(auto& e : s_init_queue)
                e();
            s_init_queue.clear();
        }

        inline static std::vector<std::jthread> call_queue_thr(){
            s_has_initialized  = true;
            std::vector<std::jthread> threads{s_init_queue.size()};
            for(size_t i = 0; i < s_init_queue.size(); i++)
                threads[i] = std::jthread{s_init_queue[i]};
            s_init_queue.clear();
            return threads;
        }

        inline static void after_gl(std::function<void(void)>&& fn){
            if(!s_has_initialized) s_init_queue.emplace_back(std::move(fn));
            else fn();
        }

        std::mutex                          l_gl;
        std::mutex& opengl_mutex()          {return l_gl;}

        void init() const{

        }
    } gl;

    struct render_t{


        void init() const{}
    } render;



    void init(){
        core.init();
        gl.init();
        render.init();
    }



    appstate_t(const appstate_t&) = delete;
    appstate_t& operator=(const appstate_t&) = delete;

    appstate_t() = default;
};