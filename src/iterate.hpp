#pragma once
#include "appstate.hpp"
#include "globals.hpp"
#include <SDL3/SDL_init.h>



    //move positions etc
inline bool update(appstate_t& state){

    return false;
}

    //all of the draws etc
inline bool draw(appstate_t& state){

    return false;
}


inline SDL_AppResult iterate(appstate_t& state){
    return update(state) || draw(state) ? SDL_APP_FAILURE : SDL_APP_CONTINUE;
}