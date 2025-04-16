#pragma once
#include "gl/shader_enums.hpp"
#include "shader/def_shader.hpp"

namespace noise {
    PRG_DEFINE(
        perlin, (16, 16), 
        
        (("/home/pikku/code/3d-sdl3/ass/shaders/noise/perlin.comp", gl::enums::shader::COMPUTE)), 

        ((seed, uint))
        ((frequency, float))
        ((octave_count, int))
        ((persistence, float))
        ((lacunarity, float)), 
        
        {}
    )
}