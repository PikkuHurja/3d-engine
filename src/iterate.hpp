#pragma once
#include "appstate.hpp"
#include "globals.hpp"
#include <SDL3/SDL_init.h>
#include <iostream>
#include <opengl-framework/Wrappers/GLEW.hpp>

inline void update_camera(){
    const bool* keystate = SDL_GetKeyboardState(nullptr);
    const float dt = appstate_t::_S_ActiveState->time.delta_timef();
    const float slow = 1 * dt;
    const float fast = 256 * dt;
    float movement = keystate[SDL_SCANCODE_LSHIFT] ? fast : slow;
    if(keystate[SDL_SCANCODE_W]){
        primary_camera.translation() += movement*primary_camera.forward();
    }else if(keystate[SDL_SCANCODE_S]){
        primary_camera.translation() += movement*primary_camera.backward();
    }
    if(keystate[SDL_SCANCODE_D]){
        primary_camera.translation() += movement*primary_camera.rightward();
    }else if(keystate[SDL_SCANCODE_A]){
        primary_camera.translation() += movement*primary_camera.leftward();
    }

    if(keystate[SDL_SCANCODE_SPACE]){
        primary_camera.translation() += movement*primary_camera.upward();
    }else if(keystate[SDL_SCANCODE_C]){
        primary_camera.translation() += movement*primary_camera.downward();
    }
    
    primary_camera.v_rotation = glm::quat{glm::vec3{primary_camera_pitch, primary_camera_yaw, 0.f}};
    primary_camera.refresh();
}

    //move positions etc
inline bool update(appstate_t& state){
    state.time.update();

    update_camera();
    return false;
}

    //all of the draws etc
inline bool draw(appstate_t& state){
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    
    point_lights.debug_draw(point_lights_debug_vao, primary_camera);

    SDL::GL::SwapWindow(*state.core.p_window);
    return false;
}


inline SDL_AppResult iterate(appstate_t& state){
    if(update(state)) return SDL_APP_FAILURE;
    if(draw(state)) return SDL_APP_FAILURE;
    return SDL_APP_CONTINUE;
}