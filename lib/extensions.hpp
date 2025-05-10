#pragma once

#include "gl/texture_enums.hpp"
namespace gl::ext {
    constexpr const char* blend_equation_advanced = "GL_KHR_blend_equation_advanced";
    constexpr const char* blend_equation_advanced_coherent = "GL_KHR_blend_equation_advanced_coherent";

    inline static bool is_supported(const char* str){
        return glewIsExtensionSupported(str);
    }
}
