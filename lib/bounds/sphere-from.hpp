#pragma once
#include "sphere.hpp"
#include "axis-alinged-box.hpp"
#include <glm/ext/vector_float3.hpp>
#include <glm/geometric.hpp>

template<>
inline sphere_t sphere_t::from(const glm::vec3& position, const float& radius){
    return sphere_t{position, radius};
}

template<>
inline sphere_t sphere_t::from(const axis_alinged_box_t& box){
    glm::vec3 center = box.center();
    return sphere_t::from(center, glm::length(box.max-center));
}

