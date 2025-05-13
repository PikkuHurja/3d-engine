#pragma once

    //axis_alinged_bounding_box
#include <glm/ext/vector_float3.hpp>
struct axis_alinged_box_t{
    glm::vec3 min;
    glm::vec3 max;
    glm::vec3 center()const{return (min+max)/2.f;}

    template<typename ...T>
    static axis_alinged_box_t from(const T&...);
};