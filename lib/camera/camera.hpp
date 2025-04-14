#pragma once
#include "gl/shader_spec.hpp"
#include "obj/transform.hpp"
#include <cmath>
#include <cstddef>
#include <gl/uniform_buffer.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/quaternion_common.hpp>
#include <glm/ext/quaternion_float.hpp>
#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/geometric.hpp>
#include <glm/matrix.hpp>
#include <glm/trigonometric.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <iostream>

#include "projection.hpp"


//using ubCamera_t = gl::uniform_buffer<glm::vec3,glm::vec3,glm::vec3,glm::vec3,glm::mat4,glm::mat4,glm::mat4,glm::mat4,glm::mat4,glm::mat4>;
    //problem: position and such are stored twice...
struct camera_t : transform, projection{
    using  buffer = gl::uniform_buffer<
        glm::vec3,
        glm::vec3,
        glm::vec3,
        glm::vec3,
        glm::mat4,
        glm::mat4,
        glm::mat4,
        glm::mat4,
        glm::mat4,
        glm::mat4
    >;

    enum ub_index{
        POSITION,
        DIRECTION_FORWARD_NORMALIZED,
        DIRECTION_UP_NORMALIZED,
        DIRECTION_RIGHT_NORMALIZED,

        VIEW,
        VIEW_INVERSE,

        PROJECTION,
        PROJECTION_INVERSE,

        PROJECTION_VIEW,
        PROJECTION_VIEW_INVERSE,
    };



    camera_t(std::nullptr_t):gl_ub_camera{nullptr}{}
    camera_t():gl_ub_camera(gl::shader_spec::ubCamera){}

    void create(const transform& t, const projection& p){
        static_cast<transform&>(*this) = t;
        static_cast<projection&>(*this) = p;
        calculate(true);
        
        if(!gl_ub_camera)
            gl_ub_camera.create(gl::shader_spec::ubCamera);

    }

    void bind()                     {gl_ub_camera.bind();}


    void upload(){
        gl_ub_camera.upload();
    }
    void refresh(){
        if(calculate())
            upload();
    }

        //returns if changed
    bool calculate(bool override = false){
        bool transform_changed = gl_transform != static_cast<transform&>(*this);
        bool projection_changed = gl_projection != static_cast<projection&>(*this);

        if(transform_changed || override){

            if(gl_transform.v_translation != v_translation || override){
                gl_ub_camera.set<POSITION>(transform::translation());
            }
            
            if(gl_transform.v_rotation != v_rotation || override){
                gl_ub_camera.set<DIRECTION_FORWARD_NORMALIZED>  (transform::forward()   );
                gl_ub_camera.set<DIRECTION_UP_NORMALIZED>       (transform::upward()    );
                gl_ub_camera.set<DIRECTION_RIGHT_NORMALIZED>    (transform::rightward() );
            }
            gl_ub_camera.set<VIEW_INVERSE>  (model_matrix());
            gl_ub_camera.set<VIEW>          (glm::inverse(gl_ub_camera.get<VIEW_INVERSE>()));
        }
        if(projection_changed || override){
            gl_ub_camera.set<PROJECTION>(projection::projection_matrix());
            gl_ub_camera.set<PROJECTION_INVERSE>(glm::inverse(gl_ub_camera.get<PROJECTION>()));
        }
        if(transform_changed || projection_changed || override){
            gl_ub_camera.set<PROJECTION_VIEW>(gl_ub_camera.get<PROJECTION>() * gl_ub_camera.get<VIEW>());
            gl_ub_camera.set<PROJECTION_VIEW_INVERSE>(glm::inverse(gl_ub_camera.get<PROJECTION_VIEW>()));
        }

        gl_transform  = *this;
        gl_projection = *this;
        return transform_changed || projection_changed || override;
    }

        //in gpu
    buffer      gl_ub_camera;
    transform   gl_transform{glm::vec3{NAN,NAN,NAN}};
    projection  gl_projection{nullptr};

};







