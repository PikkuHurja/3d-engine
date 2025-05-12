#pragma once
#include "globals.hpp"
#include "appstate.hpp"
inline SDL_AppResult event(appstate_t& state, SDL_Event& event){
    return SDL_APP_CONTINUE;
}