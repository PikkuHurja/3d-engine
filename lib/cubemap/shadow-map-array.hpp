#pragma once




#include "cubemap/cube-map-array.hpp"
#include "gl/texture_enums.hpp"
#include <glm/ext/vector_uint2.hpp>

/*
    Remember to invalidate shadow maps when the light is moved
*/
struct shadow_map_array : cube_map_array{
    inline void create(const uint& resolution, const uint& count, const gl::enums::texture::format_storage& storage_type        = gl::enums::texture::STORAGE_DEPTH_COMPONENT24){
        cube_map_array::create(resolution, count, storage_type);
    }
    inline void create(const glm::uvec2& resolution, const uint& count, const gl::enums::texture::format_storage& storage_type  = gl::enums::texture::STORAGE_DEPTH_COMPONENT24){
        cube_map_array::create(resolution, count, storage_type);
    }
};