#pragma once
#include "axis-alinged-box.hpp"
#include <glm/ext/scalar_common.hpp>
#include <glm/ext/vector_float2.hpp>

template<>
inline axis_alinged_box_t axis_alinged_box_t::from(const glm::vec3& p0, const glm::vec3& p1){
    return axis_alinged_box_t{glm::min(p0, p1), glm::max(p0, p1)}; 
}