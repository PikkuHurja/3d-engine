#pragma once

#include "gl/shader_enums.hpp"
#include "shader/def_shader.hpp"


namespace shader {

    PRG_DEFINE(terrain_gen, (16, 16), (("/home/pikku/code/3d-sdl3/ass/shaders/terrain-comp/gen/gen.comp", gl::enums::shader::COMPUTE)), 
    
        ((access_offset, uint))
        ((chunck_position, glm::ivec2))
        ((vertex_count, uint))
        ((chunck_size, float))
        ((height_map_strenght, float))
        
        ((noise_seed, uint))
        ((noise_frequency, float))
        ((noise_octave_count, int))
        ((noise_persistence, float))
        ((noise_lacunarity, float))
    
    , {})

}