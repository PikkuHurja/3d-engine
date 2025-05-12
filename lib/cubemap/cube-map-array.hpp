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


    inline operator bool()const{return _M_CubemapTexture;}

    inline const uint&          count(){return _M_CubemapTexture.texture_size().z;}
    inline const glm::uvec2&    resolution(){return reinterpret_cast<const glm::uvec2&>(_M_CubemapTexture.texture_size());}


    inline       gl::texture&   texture()               {return _M_CubemapTexture;}
    inline const gl::texture&   texture()       const   {return _M_CubemapTexture;}

    inline operator             gl::texture&()          {return _M_CubemapTexture;}
    inline operator const       gl::texture&()  const   {return _M_CubemapTexture;}

    inline void create(const uint& resolution,const uint& count, const gl::enums::texture::format_storage& storage_type){
        create(glm::uvec2{resolution}, count, storage_type);
    }
    inline void create(const glm::uvec2& resolution, const uint& count, const gl::enums::texture::format_storage& storage_type){
        _M_CubemapTexture.create(gl::enums::texture::TextureCubeMapArray, glm::uvec3{resolution, count}, storage_type, 1);

        _M_CubemapTexture.parameter(gl::enums::texture::TEXTURE_MIN_FILTER, GL_LINEAR);
        _M_CubemapTexture.parameter(gl::enums::texture::TEXTURE_MAG_FILTER, GL_LINEAR);
        _M_CubemapTexture.parameter(gl::enums::texture::TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        _M_CubemapTexture.parameter(gl::enums::texture::TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        _M_CubemapTexture.parameter(gl::enums::texture::TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    }
};