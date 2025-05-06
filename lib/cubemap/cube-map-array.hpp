#pragma once


#include "gl/texture.hpp"
#include "gl/texture_enums.hpp"
#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_uint2.hpp>

/*
TBH i have no clue what are the benefits / usecases...


// To render to face `f` of cubemap `c`:
int layer = c * 6 + f;
glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, tex, 0, layer);



uniform samplerCubeArray cubemapArray;
vec4 color = texture(cubemapArray, vec4(dir, float(cubemapIndex)));
*/
struct cube_map_array{
    gl::texture _M_CubemapTexture;

    inline void create(const glm::vec2& resolution, uint count, const gl::enums::texture::format_storage& storage_type){
        _M_CubemapTexture.create(gl::enums::texture::TextureCubeMapArray, glm::uvec3{resolution, count}, storage_type, 1);

        _M_CubemapTexture.parameter(gl::enums::texture::TEXTURE_MIN_FILTER, GL_LINEAR);
        _M_CubemapTexture.parameter(gl::enums::texture::TEXTURE_MAG_FILTER, GL_LINEAR);
        _M_CubemapTexture.parameter(gl::enums::texture::TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        _M_CubemapTexture.parameter(gl::enums::texture::TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        _M_CubemapTexture.parameter(gl::enums::texture::TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    }
};