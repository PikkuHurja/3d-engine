#pragma once
#include "gl/shader_enums.hpp"
#include "shader/def_shader.hpp"
namespace noise {
    PRG_DEFINE(
        perlin_tile, (16, 16), 
        
        (("/home/pikku/code/3d-sdl3/ass/shaders/noise/perlin-tileable.comp", gl::enums::shader::COMPUTE)), 
        
        ((seed, uint))
        ((frequency, int))
        ((octave_count, int))
        ((persistence, float))
        ((lacunarity, float))
        ((chunk_position, glm::ivec2))
        , {})
}