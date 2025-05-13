#pragma once

#include "appstate.hpp"
#include "event.hpp"
#include <iostream>


inline void quit(appstate_t* state, SDL_AppResult result){
    std::cout << "Quit called on " << result << "!\n";
    set_camera_lock(false);
    delete state;
}