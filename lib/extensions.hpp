#pragma once

#include "gl/texture_enums.hpp"
namespace gl::ext {
    constexpr const char* blend_equation_advanced = "GL_KHR_blend_equation_advanced";

    inline static bool is_supported(const char* str){
        return glewIsExtensionSupported(blend_equation_advanced);
    }
}
