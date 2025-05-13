#pragma once
#include "globals.hpp"
#include "appstate.hpp"
#include <SDL3/SDL_events.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_scancode.h>


inline void set_camera_lock(bool on){
    camera_locked = on;
    if(!SDL_SetWindowRelativeMouseMode(*appstate_t::_S_ActiveState->core.p_window, on))
        std::cerr << SDL_GetError() << '\n';
    if(!SDL_SetWindowMouseGrab(*appstate_t::_S_ActiveState->core.p_window, on))
        std::cerr << SDL_GetError() << '\n';
}
inline void toggle_camera_lock(){set_camera_lock(!camera_locked);}
inline void event_camera(SDL_Event& event){
        //rotate camere
    if(event.type == SDL_EVENT_MOUSE_MOTION){
        primary_camera_yaw      +=  event.motion.xrel *   primary_camera_sensitivity;
        primary_camera_pitch    +=  event.motion.yrel *   primary_camera_sensitivity;
    }
    if(event.type == SDL_EventType::SDL_EVENT_KEY_DOWN){
        if(event.key.scancode == SDL_SCANCODE_P)
            std::cout << "primary_camera.v_translation: [" << primary_camera.v_translation.x<< ", " << primary_camera.v_translation.y << ", " << primary_camera.v_translation.z << "]\n";
        else if(event.key.scancode == SDL_SCANCODE_L) //toggle camera
            toggle_camera_lock();
    }
}

inline SDL_AppResult event(appstate_t& state, SDL_Event& event){
    if(event.type == SDL_EVENT_QUIT)
        return SDL_APP_SUCCESS;
    event_camera(event);
    return SDL_APP_CONTINUE;
}