#pragma once
#include "cubemap/skybox.hpp"
#include "gl/vertex_array.hpp"
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



float primary_camera_pitch = 0, primary_camera_yaw = 0, primary_camera_sensitivity = -0.001;
bool camera_locked = false;
camera_t primary_camera{nullptr};
