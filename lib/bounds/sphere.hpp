#pragma once


#include <glm/ext/vector_float3.hpp>
struct sphere_t{
    glm::vec3 _M_Center;
    float _M_Radius;

    template<typename ...T>
    static sphere_t from(const T&...);
};