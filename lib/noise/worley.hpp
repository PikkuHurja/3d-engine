#pragma once

#include "gl/shader_enums.hpp"
#include "shader/def_shader.hpp"

namespace noise {
    PRG_DEFINE(worley, (16, 16), (("/home/pikku/code/3d-sdl3/ass/shaders/noise/worley.comp", gl::enums::shader::COMPUTE)), ((seed, uint))((point_count, uint)), {})
}
