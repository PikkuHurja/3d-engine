#pragma once

#include "appstate.hpp"
inline void quit(appstate_t* state, SDL_AppResult result){
    delete state;
}