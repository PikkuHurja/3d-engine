#pragma once

#include "gl/texture_enums.hpp"
namespace gl::ext {
    constexpr const char* blend_equation_advanced = "GL_KHR_blend_equation_advanced";
    constexpr const char* blend_equation_advanced_coherent = "GL_KHR_blend_equation_advanced_coherent";
    constexpr const char* shader_framebuffer_fetch = "GL_EXT_shader_framebuffer_fetch";
    
    inline static bool is_supported(const char* str){
        return glewIsExtensionSupported(str);
    }
}
