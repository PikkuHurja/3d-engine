#pragma once


#include "camera/camera.hpp"
#include "camera/perspective.hpp"
#include "camera/projection.hpp"
#include "gl/texture.hpp"
#include "gl/texture_enums.hpp"
#include "obj/transform.hpp"
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/ext/matrix_clip_space.hpp>


/*
use "glNamedFramebufferTextureLayer" when binding to a render target
*/

struct cube_map{
    inline static constexpr glm::vec3 directions[6] = {
        glm::vec3( 1, 0, 0),  // +X
        glm::vec3(-1, 0, 0),  // -X
        glm::vec3( 0, 1, 0),  // +Y
        glm::vec3( 0,-1, 0),  // -Y
        glm::vec3( 0, 0, 1),  // +Z
        glm::vec3( 0, 0,-1)   // -Z
    };
    inline static constexpr glm::vec3 up_vectors[6] = {
        glm::vec3(0,-1, 0),   // +X
        glm::vec3(0,-1, 0),   // -X
        glm::vec3(0, 0, 1),   // +Y
        glm::vec3(0, 0,-1),   // -Y
        glm::vec3(0,-1, 0),   // +Z
        glm::vec3(0,-1, 0)    // -Z
    };
    inline static const glm::quat rotations[6] = {
            //maby should use RH?
        glm::quat_cast(glm::lookAtRH(glm::vec3(0), directions[0], up_vectors[0])),  // +X
        glm::quat_cast(glm::lookAtRH(glm::vec3(0), directions[1], up_vectors[1])),  // -X
        glm::quat_cast(glm::lookAtRH(glm::vec3(0), directions[2], up_vectors[2])),   // +Y
        glm::quat_cast(glm::lookAtRH(glm::vec3(0), directions[3], up_vectors[3])),  // -Y
        glm::quat_cast(glm::lookAtRH(glm::vec3(0), directions[4], up_vectors[4])),  // +Z
        glm::quat_cast(glm::lookAtRH(glm::vec3(0), directions[5], up_vectors[5]))   // -Z
    };

    inline static constexpr uint face_count = 6;
    inline static constexpr float near_plane = .1;
    inline static constexpr float far_plane  = 500;
    inline static projection_t capture_projection{perspective_t{glm::radians(90.0f), 1.0f, near_plane, far_plane}};// = glm::perspective(glm::radians(90.0f), 1.0f, near_plane, far_plane);
    inline static glm::mat4    capture_perspective = glm::perspective(glm::radians(90.0f), 1.0f, near_plane, far_plane);

    gl::texture     _M_CubemapTexture;
    
    inline gl::texture& texture()              {return _M_CubemapTexture;}
    inline const gl::texture& texture() const  {return _M_CubemapTexture;}
    inline operator gl::texture&()             {return _M_CubemapTexture;}
    inline operator const gl::texture&()const  {return _M_CubemapTexture;}

    inline void create(const float& resolution, const gl::enums::texture::format_storage& storage_type){
        return create(glm::vec2{resolution}, storage_type);
    }
    inline void create(const glm::uvec2& resolution, const gl::enums::texture::format_storage& storage_type){
        _M_CubemapTexture.create(gl::enums::texture::type::TextureCubeMap, resolution, storage_type,1);

        _M_CubemapTexture.parameter(gl::enums::texture::TEXTURE_MIN_FILTER, GL_LINEAR);
        _M_CubemapTexture.parameter(gl::enums::texture::TEXTURE_MAG_FILTER, GL_LINEAR);
        _M_CubemapTexture.parameter(gl::enums::texture::TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        _M_CubemapTexture.parameter(gl::enums::texture::TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        _M_CubemapTexture.parameter(gl::enums::texture::TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    }


    inline static struct ::transform_t transform(const glm::vec3& position, const uint face_index){
        return ::transform_t{position, rotations[face_index], glm::vec3{1}};
    }

    inline static const ::projection_t& projection(){
        return capture_projection;
    }
        //needs to be calculated and whatnot
    inline static void set_camera(camera_t& cam, const glm::vec3& position, const uint face_index){
        cam = projection();
        cam = transform(position, face_index);
    }
    inline static glm::mat4 view(const glm::vec3& position, const uint face_index){
        return glm::lookAtRH(position, position + directions[face_index], up_vectors[face_index]);
    }
    inline static void views(glm::mat4 view_mat[face_count], const glm::vec3& position){
        for(size_t i = 0; i < face_count; i++){
            view_mat[i] = view(position, i);
        }
    }
    inline static const glm::mat4& perspective(){
        return capture_perspective;
    }
};