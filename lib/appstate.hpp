#pragma once

#include <SDL/SDL.h>
#include <SDL/Window.h>
#include <SDL3/SDL_video.h>
#include <chrono>
#include <iostream>
#include <opengl-framework/Wrappers/GLEW.hpp>
#include <SDL/GL.h>
#include <functional>
#include <memory>
#include <mutex>
#include <thread>
#include <utility>
#include <vector>
#include <renderdoc_app.h>
#include <dlfcn.h>



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

            p_window.reset(new SDL::Window{"", {1280, 720}, SDL_WINDOW_OPENGL | SDL_WINDOW_INPUT_FOCUS});
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
            call_queue();
        }
    } gl;


    struct debug_t{
        
        struct opengl_t{
            void *dl_handle = nullptr;
            RENDERDOC_API_1_1_2 *rdoc_api = nullptr;

            inline void capture_begin(){
                if(rdoc_api) 
                    rdoc_api->StartFrameCapture(nullptr, nullptr);
            }
            inline void capture_end(){
                if(rdoc_api) 
                    rdoc_api->EndFrameCapture(nullptr, nullptr);
            }
            void init(){
                    ////////////// GL BOILERPLATE //////////////    
                std::cout << "Using opengl: " << glGetString(GL_VERSION) << '\n';
                glEnable(GL_DEBUG_OUTPUT);
                glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
                glDebugMessageCallback([](GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam) {
                    std::cerr << "GL Debug["<<id<<"]{src: "<< source << ", type: " << type << ", lvl: "<< severity <<" }: " << std::string_view{message, static_cast<size_t>(length < 0 ? 0 : length)} << std::endl;
                }, nullptr);
                glClearColor(.1,0, .1, 1);

                if(dl_handle) deinit();

                dl_handle = dlopen("librenderdoc.so", RTLD_NOW | RTLD_NOLOAD);
                if (dl_handle) {
                    pRENDERDOC_GetAPI getApi = (pRENDERDOC_GetAPI)dlsym(dl_handle, "RENDERDOC_GetAPI");
                    if (getApi) getApi(eRENDERDOC_API_Version_1_1_2, (void **)&rdoc_api);
                }
            }

            void deinit(){
                if(dl_handle)
                    dlclose(dl_handle);
                dl_handle = nullptr;
                rdoc_api  = nullptr;
            }
        } opengl;

        void init(){
            opengl.init();
        }
        void deinit(){
            opengl.deinit();
        }
    } debug;

    struct render_t{
        void init() const{}
    } render;

    struct time_t{
        using c = std::chrono::steady_clock;
        using tp = c::time_point;
        using d = c::duration;
        tp begin                    = c::now();
        tp last_update              = begin;
        tp last_fixed_update        = begin;
        tp current_update           = begin;

        inline tp now() const {return current_update;}
        inline d delta_time() const {return current_update - last_update;}
        inline float delta_timef() const {return std::chrono::duration_cast<std::chrono::duration<float>>(current_update - last_update).count();}
        inline double delta_timed() const {return std::chrono::duration_cast<std::chrono::duration<double>>(current_update - last_update).count();}

        inline void update(){
            last_update = current_update;
            current_update = c::now();
        }
    } time;

    void init(){
        core.init();
        debug.init();
        gl.init();
        render.init();
    }



    appstate_t(const appstate_t&) = delete;
    appstate_t& operator=(const appstate_t&) = delete;

    appstate_t() = default;
};