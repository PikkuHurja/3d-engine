#pragma once
#include "appstate.hpp"
#include "globals.hpp"
#include <SDL3/SDL_init.h>


inline bool init_shaders(){

    return false;
}
inline bool init_cameras(){

    return false;
}
inline bool init_lights(){

    return false;
}
inline bool init_skybox(){

    return false;
}
inline bool init_terrain(){

    return false;
}

inline SDL_AppResult init(appstate_t** pp_state, int argc, char** argv){
    appstate_t& state = *(appstate_t::_S_ActiveState = *pp_state = new appstate_t());
    state.init();

    if(init_shaders())
        return SDL_APP_FAILURE;
    
    if(init_cameras())
        return SDL_APP_FAILURE;

    if(init_lights())
        return SDL_APP_FAILURE;

    if(init_skybox())
        return SDL_APP_FAILURE;
    
    return SDL_APP_CONTINUE;
}