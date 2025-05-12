#pragma once

#include "cubemap/cube-map.hpp"
#include <array>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/ext/vector_float4.hpp>
#include <sys/types.h>

struct point_light{
    inline static constexpr uint npos = -1;

    uint array_index = npos; //when accessing cubemap arrays
    uint padding[3];
    glm::vec4 position_radius = glm::vec4{0,0,0, 25}; //position: xyz, cutoff-radius: w;
    glm::vec4 color_intensity = glm::vec4{1,1,1, 1}; //color: rgb, light-intensity: w;


    bool regen_matricies(const point_light& l)const{return position() != l.position();}

    const uint& index()const{return array_index;}
    const uint& index(const uint& set_index){return array_index = set_index;}

    glm::vec3& position(){return reinterpret_cast<glm::vec3&>(position_radius);}
    glm::vec3& color(){return reinterpret_cast<glm::vec3&>(color_intensity);}


    const glm::vec3& position()const {return reinterpret_cast<const glm::vec3&>(position_radius);}
    const glm::vec3& color()const {return reinterpret_cast<const glm::vec3&>(color_intensity);}

    float& radius(){return position_radius.w;}
    float& intensity(){return color_intensity.w;}

    void matricies(glm::mat4 matrixes[6]){cube_map::perspective_views(matrixes, glm::vec3{position_radius});}
    void matricies(std::array<glm::mat4, 6> matrixes){cube_map::perspective_views(matrixes.data(), glm::vec3{position_radius});}

    bool operator==(const point_light& l)const{return array_index == l.array_index && position_radius == l.position_radius && color_intensity == l.color_intensity;}
    bool operator!=(const point_light& l){return !(*this == l);}
    
};