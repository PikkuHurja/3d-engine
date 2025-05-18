#pragma once
#include "cubemap/skybox.hpp"
#include "gl/vertex_array.hpp"
#include "gl_mesh_interleaved.hpp"
#include <glm/detail/qualifier.hpp>
#include <glm/ext/vector_float4.hpp>
#include <ostream>
#include <vector>
#define SDL_MAIN_USE_CALLBACKS 1
#define sdl_ext extern "C" 

#include <GL/glew.h>

#include <SDL3/SDL_main.h>
#include <SDL3/SDL_init.h>
#include <SDL3/SDL_video.h>
#include <SDL3/SDL_events.h>

#include <exception>
#include <iostream>

#include "lights/point-light-array.hpp"
#include "lights/point-light.hpp"

using point_light_array = point_light_array_t<point_light>;

gl::vertex_array  point_lights_debug_vao{nullptr};
point_light_array point_lights;

std::vector<rt_gl_mesh_interleaved_t> teapot_meshes;

template<typename T, glm::length_t L, glm::qualifier Q>
std::ostream& operator <<(std::ostream& os, const glm::vec<L, T, Q>& v){
    os << '[';
    for(size_t i = 0; i < L; i++){
        os << v[i];
        if(i+1 == L) break;
        os << ", ";
    }
    return os << ']';
}

std::ostream& operator <<(std::ostream& os, const glm::vec4& v){
    os << '[';
    for(size_t i = 0; i < 4; i++){
        os << v[i];
        if(i+1 == 4) break;
        os << ", ";
    }
    return os << ']';
}
#include "datatypes/multi-instancer.hpp"

using shitbox_5000 = multi_instancer<int, glm::vec4>;
shitbox_5000 instances[64];



float primary_camera_pitch = 0, primary_camera_yaw = 0, primary_camera_sensitivity = -0.001;
bool camera_locked = false;
camera_t primary_camera{nullptr};
