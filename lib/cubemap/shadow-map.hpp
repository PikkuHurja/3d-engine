#pragma once

#include "cube-map.hpp"
#include "gl/texture_enums.hpp"

struct shadow_map : cube_map{
    inline void create(const uint& resolution, const gl::enums::texture::format_storage& storage_type = gl::enums::texture::format_storage::STORAGE_DEPTH_COMPONENT24){
        return cube_map::create(glm::uvec2{resolution}, storage_type);
    }
    inline void create(const glm::uvec2& resolution, const gl::enums::texture::format_storage& storage_type = gl::enums::texture::format_storage::STORAGE_DEPTH_COMPONENT24){
        return cube_map::create(resolution, storage_type);
    }
};