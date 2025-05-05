#pragma once
#include "appstate.hpp"
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
#include <mutex>

#include "projection.hpp"


//using ubCamera_t = gl::uniform_buffer<glm::vec3,glm::vec3,glm::vec3,glm::vec3,glm::mat4,glm::mat4,glm::mat4,glm::mat4,glm::mat4,glm::mat4>;
    //problem: position and such are stored twice...
struct camera_t : transform_t, projection_t{
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
    camera_t& operator=(const transform_t& t){
        static_cast<transform_t&>(*this) = t;
        return *this;
    }
    camera_t& operator=(const projection_t& p){
        static_cast<projection_t&>(*this) = p;
        return *this;
    }


    camera_t(std::nullptr_t):gl_ub_camera{nullptr}{}
    camera_t():gl_ub_camera(gl::shader_spec::ubCamera){}
    camera_t(const transform_t& t, const projection_t& p):camera_t(){
        create(t, p);
    }

    void create(const transform_t& t, const projection_t& p){
        static_cast<transform_t&>(*this) = t;
        static_cast<projection_t&>(*this) = p;
        calculate(true);
        
        if(!gl_ub_camera){
            std::lock_guard l{appstate_t::_S_ActiveState->gl.opengl_mutex()};
            gl_ub_camera.create(gl::shader_spec::ubCamera);
        }else{
            upload();
        }

    }

    void bind()                     {gl_ub_camera.bind();}


    void upload(){
        std::lock_guard l{appstate_t::_S_ActiveState->gl.opengl_mutex()};
        gl_ub_camera.upload();
    }
    void refresh(){
        if(calculate())
            upload();
    }

        //returns if changed
    bool calculate(bool override = false){
        bool transform_changed = gl_transform != static_cast<transform_t&>(*this);
        bool projection_changed = gl_projection != static_cast<projection_t&>(*this);

        if(transform_changed || override){

            if(gl_transform.v_translation != v_translation || override){
                gl_ub_camera.set<POSITION>(transform_t::translation());
            }
            
            if(gl_transform.v_rotation != v_rotation || override){
                gl_ub_camera.set<DIRECTION_FORWARD_NORMALIZED>  (transform_t::forward()   );
                gl_ub_camera.set<DIRECTION_UP_NORMALIZED>       (transform_t::upward()    );
                gl_ub_camera.set<DIRECTION_RIGHT_NORMALIZED>    (transform_t::rightward() );
            }
            gl_ub_camera.set<VIEW_INVERSE>  (model_matrix());
            gl_ub_camera.set<VIEW>          (glm::inverse(gl_ub_camera.get<VIEW_INVERSE>()));
        }
        if(projection_changed || override){
            gl_ub_camera.set<PROJECTION>(projection_t::projection_matrix());
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
    transform_t   gl_transform{glm::vec3{NAN,NAN,NAN}};
    projection_t  gl_projection{nullptr};

};







