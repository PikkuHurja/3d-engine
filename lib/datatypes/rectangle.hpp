#pragma once



#include "datatypes/instancer.hpp"
#include "draw/draw.hpp"
#include "gl/buffer.hpp"
#include "gl/buffer_enums.hpp"
#include "gl/draw_enums.hpp"
#include "gl/shader_spec.hpp"
#include "gl/vertex_array.hpp"
#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float3.hpp>
#include <iostream>

struct rectangle_verticies{
    struct vertex{
        glm::vec3 _aVertex;
        glm::vec2 _aUV;
    };
    vertex _Verticies[4]{
        vertex{glm::vec3{-1,-1,0},  glm::vec2{0, 0}},
        vertex{glm::vec3{-1,1,0},   glm::vec2{0, 1}},
        vertex{glm::vec3{1,1,0},    glm::vec2{1, 1}},
        vertex{glm::vec3{1,-1,0},   glm::vec2{1, 0}},
    };
};
struct rectangle : instancer<rectangle_verticies> {
public:
    using inst = instancer<rectangle_verticies>;
    using inst::instancer;
    using inst::operator bool;

    inline static void draw_all(){
        auto size = inst::buffer_size();
        if(!size){
            std::cerr << "No 'rectangle::draw_all()' draw, since data.size == 0\n";
            return;
        }
        if(modified()) std::cerr << "'rectangle::draw_all()' there is modified data, that has not been uploaded to the GPU\n";
        if(!_S_GPURectanglesVAO) _init_vao();
        _S_GPURectanglesVAO.bind();
        gl::draw_one(gl::enums::TRIANGLE_FAN, 0, size*4); //4 verticies per instance
    }
private:
    inline static void _init_vao(){
        _S_GPURectanglesVAO.create();
        _S_GPURectanglesVAO.bind();
        auto& data = inst::gpu_buffer();
        if(!data) data.create();
        data.bind(gl::enums::buffer::ARRAY_BUFFER);
        
        _S_GPURectanglesVAO.enable_attribute(gl::shader_spec::aVertex);
        _S_GPURectanglesVAO.enable_attribute(gl::shader_spec::aUV);

        data.attribute(gl::shader_spec::aVertex, 3, gl::enums::buffer::FLOAT, sizeof(rectangle_verticies::vertex), 0, 0);
        data.attribute(gl::shader_spec::aUV, 2, gl::enums::buffer::FLOAT, sizeof(rectangle_verticies::vertex), 0, sizeof(glm::vec3));

        _S_GPURectanglesVAO.unbind();
        data.unbind(gl::enums::buffer::ARRAY_BUFFER);
    }
    inline static gl::vertex_array  _S_GPURectanglesVAO{nullptr};
};