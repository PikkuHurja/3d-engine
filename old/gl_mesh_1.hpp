#pragma once


#include "gl/buffer_enums.hpp"
#include "gl/vertex_array.hpp"
#include "gl/buffer.hpp"
#include "gl/draw_enums.hpp"
#include "gl/shader_spec.hpp"
#include <glm/glm.hpp>
#include <cstddef>
#include <type_traits>


template<typename T>
concept vertex_like = std::is_same_v<std::remove_all_extents_t<T>, float> || std::is_same_v<std::remove_all_extents_t<T>, glm::vec2> || std::is_same_v<std::remove_all_extents_t<T>, glm::vec3>;

template<vertex_like T>
inline static constexpr int vertex_dimention(){
    return sizeof(T)/sizeof(float);
}


enum mesh_features_t{
    VERTICIES,
    TEXTURE_MAP,
    NORMAL_MAP,
    TANGENT_BITANGENT_MAP,
    INDECIES, //yes
};

    //only vertexdata and vao
struct gl_mesh_t{
    gl::vertex_array gl_vao{nullptr};
    gl::typed_buffer<gl::enums::buffer::ARRAY_BUFFER> gl_verticies{nullptr};
    void draw(gl::enums::drawmode dm, GLint beg, GLsizei count){
        gl_vao.bind();
        glDrawArrays(dm, beg, count);
    }
    void drawInstanced(gl::enums::drawmode dm, GLint beg, GLsizei count, GLsizei instance_count){
        gl_vao.bind();
        glDrawArraysInstanced(dm, beg, count, instance_count);
    }

    void create(const vertex_like auto* const vertices, size_t vertex_count, gl::enums::buffer::usage vertex_usage = gl::enums::buffer::usage::STATIC_DRAW){
        gl_vao.create();
        gl_vao.bind();
        gl_verticies.create();
        gl_verticies.bind();
        gl_verticies.data(vertices, vertex_count, vertex_usage);
        gl_verticies.attribute(gl::shader_spec::aVertex, sizeof(*vertices)/sizeof(float), gl::enums::buffer::FLOAT);
        gl_vao.enable_attribute(gl::shader_spec::aVertex);
    }
};

struct gl_indecied_mesh_t : gl_mesh_t{
    gl::typed_buffer<gl::enums::buffer::ELEMENT_ARRAY_BUFFER> gl_indecies{nullptr};
    
    void create(
        const vertex_like auto* const vertices, size_t vertex_count,
        const unsigned int* indecies, size_t indecie_count, 
        gl::enums::buffer::usage vertex_usage = gl::enums::buffer::usage::STATIC_DRAW, 
        gl::enums::buffer::usage indecie_usage = gl::enums::buffer::usage::STATIC_DRAW){

        gl_mesh_t::create(vertices, vertex_count, vertex_usage);
        gl_indecies.create();
        gl_indecies.bind();
        gl_indecies.data(indecies, indecie_count, indecie_usage);
    }

    void draw(gl::enums::drawmode dm, GLint beg, size_t indecie_offset){
        gl_vao.bind();
        glDrawElements(dm, beg, GL_UNSIGNED_INT, reinterpret_cast<void*>(indecie_offset));
    }
    void drawInstanced(gl::enums::drawmode dm, GLint beg, GLsizei count, size_t indecie_offset, GLsizei instance_count){
        gl_vao.bind();
        glDrawElementsInstanced(dm, beg, count, reinterpret_cast<void*>(indecie_offset), instance_count);
    }
protected:
    using gl_mesh_t::create;
};

