#pragma once


#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/quaternion_float.hpp>
#include <glm/ext/quaternion_geometric.hpp>
#include <glm/ext/quaternion_trigonometric.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/gtc/quaternion.hpp>

struct transform{
    glm::vec3 v_translation =   glm::vec3{0.f};
    glm::quat v_rotation    =   glm::quat{1.f, 0, 0, 0};
    glm::vec3 v_scale       =   glm::vec3{1.f};

    inline bool operator ==(const transform& t)const{return t.v_translation == v_translation && t.v_rotation == v_rotation && t.v_scale == v_scale;}
    inline bool operator !=(const transform& t)const{return !(t == *this);}


        ////////////////////////////// GET //////////////////////////////

    const glm::vec3& translation()const{return v_translation;}
    const glm::quat& rotation()const{return v_rotation;}
    const glm::vec3& scale()const{return v_scale;}

    glm::vec3& translation(){return v_translation;}
    glm::quat& rotation(){return v_rotation;}
    glm::vec3& scale(){return v_scale;}

        ////////////////////////////// SET //////////////////////////////

    glm::vec3& translation(const glm::vec3& v)              {return v_translation   = v;}

    glm::quat& rotation(const glm::quat& v)                 {return v_rotation      = v;}
    glm::quat& rotation(const glm::vec3& euler)             {return v_rotation      = glm::quat{euler};}
    glm::quat& rotation(const glm::vec3& axis, float angle) {return v_rotation      = glm::angleAxis(angle, axis);}

    glm::vec3& scale(const glm::vec3& v)                    {return v_scale         = v;}
    glm::vec3& scale(const float& v)                    {return v_scale         = glm::vec3{v};}


        ////////////////////////////// APPEND / ACCUMILATE //////////////////////////////

    glm::vec3& translate(const glm::vec3& v){return v_translation += v;}
    glm::quat& rotate(const glm::quat& v){
        v_rotation = v*v_rotation;
        v_rotation = glm::normalize(v_rotation);
        return v_rotation;
    }
    glm::quat& rotate(const glm::vec3& euler){
        v_rotation = glm::quat{euler} * v_rotation;
        v_rotation = glm::normalize(v_rotation);
        return v_rotation;
    }
    glm::quat& rotate(const glm::vec3& axis, float angle) {
        v_rotation = glm::angleAxis(angle, axis)*v_rotation;
        v_rotation = glm::normalize(v_rotation);
        return v_rotation;
    }
    
     
    glm::vec3& scale_by(const glm::vec3& v){return v_scale *= v;}
    glm::vec3& scale_by(const float& v){return v_scale *= v;}


        ////////////////////////////// TO CALCULATE //////////////////////////////

    glm::vec3 rotation_euler() const {return glm::eulerAngles(v_rotation);}

    glm::vec3 forward()     const { return v_rotation * glm::vec3(0.0f, 0.0f, -1.0f); }
    glm::vec3 rightward()   const { return v_rotation * glm::vec3(1.0f, 0.0f, 0.0f); }
    glm::vec3 upward()      const { return v_rotation * glm::vec3(0.0f, 1.0f, 0.0f); }

    glm::vec3 backward()    const { return -forward();   }
    glm::vec3 leftward()    const { return -rightward(); }
    glm::vec3 downward()    const { return -upward();    }

    inline static glm::mat4 model_matrix(const glm::vec3& v_translation, const glm::quat& v_rotation, const glm::vec3& v_scale){
        auto model = glm::translate(glm::mat4(1), v_translation);
        model = model * glm::mat4_cast(v_rotation);
        model = glm::scale(model, v_scale);
        return model;
    }

    glm::mat4 model_matrix()const{
        auto model = glm::translate(glm::mat4(1), v_translation);
        model = model * glm::mat4_cast(v_rotation);
        model = glm::scale(model, v_scale);
        return model;
    }

};