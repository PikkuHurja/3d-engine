#pragma once


    //singular, non-instanced, use arrays where aplicable
#include "cubemap/cube-map.hpp"
#include "cubemap/shadow-map.hpp"
#include "gl/framebuffer.hpp"
#include "gl/framebuffer_enums.hpp"
#include "gl/texture_enums.hpp"
#include <glm/ext/vector_uint2.hpp>
struct point_light{
    

    inline static constexpr gl::enums::texture::format_storage default_shadow_map_st    = gl::enums::texture::STORAGE_DEPTH_COMPONENT24;
    inline static constexpr gl::enums::texture::format_storage default_tint_map_st      = gl::enums::texture::STORAGE_RGBA8;
    shadow_map  gl_shadow_map;
    cube_map    gl_tint_map;


    inline void create(const uint& resolution, const gl::enums::texture::format_storage& shadow_map_sf = default_shadow_map_st, const gl::enums::texture::format_storage& tint_map_sf = default_tint_map_st){
        create(glm::uvec2{resolution}, shadow_map_sf, tint_map_sf);
    }

    inline void create(const glm::uvec2& resolution, const gl::enums::texture::format_storage& shadow_map_sf = default_shadow_map_st, const gl::enums::texture::format_storage& tint_map_sf = default_tint_map_st){
        gl_shadow_map   .create(resolution, shadow_map_sf);
        gl_tint_map     .create(resolution,tint_map_sf);
    }

    inline void attach_to(gl::framebuffer& fb) const{
        fb.attach(gl_shadow_map.texture(), gl::enums::framebuffer::DEPTH_ATTACHMENT, 0);
        fb.attach(gl_tint_map.texture(), gl::enums::framebuffer::COLOR_ATTACHMENT0, 0);
    }


};