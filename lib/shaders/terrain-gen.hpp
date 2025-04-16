#pragma once

#include "gl/shader_enums.hpp"
#include "shader/def_shader.hpp"


namespace shader {

    PRG_DEFINE(terrain_gen, (16, 16), (("/home/pikku/code/3d-sdl3/ass/shaders/terrain-comp/gen/gen.comp", gl::enums::shader::COMPUTE)), 
    
        ((access_offset, uint))
        ((chunck_position, glm::ivec2))
        ((chunck_size, glm::uvec2))
        ((height_map_strenght, float))
    
    , {})

}