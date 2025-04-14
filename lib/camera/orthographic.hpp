#pragma once
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/vec2.hpp>
#include <glm/mat4x4.hpp>

struct orthographic{
    glm::vec2 size;

    inline bool operator ==(const orthographic& t)const{return t.size == size;}
    inline bool operator !=(const orthographic& t)const{return !(t == *this);}

    inline static orthographic make_default(){
        return {glm::vec2{1.f}};
    }
    glm::mat4 matrix()const{
        return glm::ortho(0.f, size.x, 0.f, size.y);
    }
};