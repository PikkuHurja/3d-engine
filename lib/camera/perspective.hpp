#pragma once
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_float4x4.hpp>

struct perspective{
    float fov_y;         // Vertical field of view in radians
    float aspect_ratio;  // Width/height ratio
    float near_plane;    // Near clipping plane
    float far_plane;     // Far clipping plane

    inline bool operator ==(const perspective& t)const{return 
        t.fov_y == fov_y &&
        t.aspect_ratio == aspect_ratio &&
        t.near_plane == near_plane &&
        t.far_plane == far_plane;
    }
    inline bool operator !=(const perspective& t)const{return !(t == *this);}

    inline static perspective make_default(){
        return perspective{glm::radians(90.f), 16.f/9.f, 0.001, 100};
    }
    glm::mat4 matrix()const{
        return glm::perspective(fov_y, aspect_ratio, near_plane, far_plane);
    }
};