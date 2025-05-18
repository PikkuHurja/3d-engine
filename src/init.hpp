#pragma once
#include "appstate.hpp"
#include "globals.hpp"
#include "lights/point-light.hpp"
#include "model/load.hpp"
#include <SDL3/SDL_init.h>
#include <glm/ext/vector_float3.hpp>
#include <glm/ext/vector_float4.hpp>
#include <iostream>


inline bool init_shaders(){
    point_lights.debug_init();
    
    return false;
}
inline bool init_cameras(){
    primary_camera.create(transform_t{glm::vec3{0}, glm::quat{1, 0, 0, 0}, glm::vec3{1}}, projection_t{perspective_t::make_default()});
    return false;
}
inline bool init_lights(){
    point_lights_debug_vao.create();
    point_lights.create(16, 1<<10, 1<<10);
    point_lights.add({0, {0}, glm::vec3{1}, 1});
    point_lights.add({0, {0}, glm::vec3{-1}, 26});
    point_lights.upload();

    point_lights.debug_bind(point_lights_debug_vao);

    return false;
}
inline bool init_skybox(){

    return false;
}
inline bool init_terrain(){

    return false;
}
inline void init_test(){
    //for(size_t i = 0; i < 20; i++){
    //    instances[i].create(shitbox_5000::vec::val_tuple{i, glm::vec4{i, i*2, i*3, i*4}});
    //}
    //std::cout << *instances << '\n';
    //shitbox_5000::upload(true);
    //std::cout<<shitbox_5000::buffer() << '\n';
    //std::cout<<shitbox_5000::gpu_buffer_capacity() << '\n';

        std::cout << "Loading teapot...\n";
    model::load("ass/models/teapot.obj", teapot_meshes);
        std::cout << "Loaded teapot!\n";
        
    std::cout << "teapot_meshes.size: " << teapot_meshes.size() << '\n';
    if(!teapot_meshes.empty())
        std::cout << "teapot_meshes[0].gl_data.id: " << teapot_meshes[0].gl_data.id() << '\n';

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
    

    init_test();
    return SDL_APP_CONTINUE;
}