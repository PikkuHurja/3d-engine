#pragma once

#include "cubemap/cube-map.hpp"
#include <array>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/ext/vector_float4.hpp>
#include <sys/types.h>

struct point_light{
    inline static constexpr uint npos = -1;
    inline static constexpr uint position_radius_offset = 16;
    inline static constexpr uint color_intensity_offset = 32;

    bool regen_matricies(const point_light& l)const{return position() != l.position();}

    const uint& index()const{return _M_ArrayIndex;}
    const uint& index(const uint& set_index){return _M_ArrayIndex = set_index;}

    glm::vec3& position(){return *reinterpret_cast<glm::vec3*>(&_M_Position);}
    glm::vec3& color(){return *reinterpret_cast<glm::vec3*>(&_M_Color);}

    const glm::vec3& position()const {return *reinterpret_cast<const glm::vec3*>(&_M_Position);}
    const glm::vec3& color()const {return *reinterpret_cast<const glm::vec3*>(&_M_Color);}

    float& radius(){return _M_Radius;}
    float& intensity(){return _M_Intensity;}

    const float& radius()const{return _M_Radius;}
    const float& intensity()const{return _M_Intensity;}

    void matricies(glm::mat4 matrixes[6]){cube_map::perspective_views(matrixes, _M_Position);}
    void matricies(std::array<glm::mat4, 6> matrixes){cube_map::perspective_views(matrixes.data(), _M_Position);}

    bool operator==(const point_light& l) const{return _M_ArrayIndex == l._M_ArrayIndex && _M_Position == l._M_Position && _M_Radius == l._M_Radius && _M_Color == l._M_Color && _M_Intensity == l._M_Intensity;}
    bool operator!=(const point_light& l) {return !(*this == l);}
    


    uint _M_ArrayIndex = npos; //when accessing cubemap arrays
    uint _M_Padding[3];
        //will be packed to a vec4
    glm::vec3 _M_Position{0.f};
    float _M_Radius{0.f};

    glm::vec3 _M_Color{1}; //color: rgb, light-intensity: w;
    float _M_Intensity{1};
};