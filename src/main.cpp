#include "appstate.hpp"
#include "event.hpp"
#include "globals.hpp"
#include "init.hpp"
#include "iterate.hpp"
#include "quit.hpp"


sdl_ext SDL_AppResult SDL_AppInit(void **appstate, int argc, char **argv)try{
    return init(reinterpret_cast<appstate_t**>(appstate), argc, argv);
}catch(const std::exception& e){
    std::cerr << "Uncaught exception at SDL_AppInit: " << e.what() << '\n'; 
    return SDL_APP_FAILURE;
}

sdl_ext SDL_AppResult SDL_AppIterate(void *appstate)try{
    return iterate(*reinterpret_cast<appstate_t*>(appstate));
}catch(const std::exception& e){
    std::cerr << "Uncaught exception at SDL_AppIterate: " << e.what() << '\n'; 
    return SDL_APP_FAILURE;
}

sdl_ext SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event* ev)try{
    return event(*reinterpret_cast<appstate_t*>(appstate), *ev);
}catch(const std::exception& e){
    std::cerr << "Uncaught exception at SDL_AppIterate: " << e.what() << '\n'; 
    return SDL_APP_FAILURE;
}

sdl_ext void SDL_AppQuit(void *appstate, SDL_AppResult result)try{
    quit(reinterpret_cast<appstate_t*>(appstate), result);
}catch(const std::exception& e){
    std::cerr << "Uncaught exception at SDL_AppIterate: " << e.what() << '\n'; 
}