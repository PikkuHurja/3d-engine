#pragma once

#include "gl/buffer_enums.hpp"
#include "gl/vertex_array.hpp"
#include "gl/buffer.hpp"
#include "gl/draw_enums.hpp"
#include "gl/shader_spec.hpp"
#include <glm/glm.hpp>
#include <cstddef>
#include <type_traits>


    //thread unsafe, since so many calls, its better to do the locking in a bigger function
    //this just handles the opengl side
    //if using indecies, you should have it as uint array, hardcoded? yes, but i dont care for the 0.00001% of the other appliactions / optimizations for shorts or bytes :3
template<typename vertex_buffer_type, typename indecie_buffer_type>
struct gl_mesh_t;



template<typename T>
concept vertex_like = std::is_same_v<std::remove_all_extents_t<T>, float> || std::is_same_v<std::remove_all_extents_t<T>, glm::vec2> || std::is_same_v<std::remove_all_extents_t<T>, glm::vec3>;

template<vertex_like T>
inline static constexpr int vertex_dimention(){
    return sizeof(T)/sizeof(float);
}


template <typename T>
concept has_constructor_create = requires(T t, const void *data, GLsizeiptr size, gl::enums::buffer::storage_flags flags) {
    { t.create(data, size, flags) };
};

template<typename vertex_buffer_type>
struct gl_mesh_t<vertex_buffer_type, void> {
    gl::vertex_array    gl_vao{nullptr};
    inline static constexpr gl::enums::buffer::type verticie_type = gl::enums::buffer::ARRAY_BUFFER;
    vertex_buffer_type  gl_verticies{nullptr}; //lets assume




    void draw(gl::enums::drawmode dm, GLint beg, GLsizei count){
        gl_vao.bind();
        glDrawArrays(dm, beg, count);
    }
    void drawInstanced(gl::enums::drawmode dm, GLint beg, GLsizei count, GLsizei instance_count){
        gl_vao.bind();
        glDrawArraysInstanced(dm, beg, count, instance_count);
    }



    void create(const vertex_like auto* const vertices, size_t vertex_count, GLsizei vertex_stride = 0)
    requires(has_constructor_create<vertex_buffer_type>)
    {
        gl_vao.create();
        gl_verticies.create(vertices, vertex_count*sizeof(*vertices), gl::enums::buffer::DYNAMIC_STORAGE);

        gl_vao.bind();
        gl_verticies.attribute(gl::shader_spec::aVertex, vertex_dimention<decltype(*vertices)>(),  GL_FLOAT, vertex_stride, GL_FALSE, 0);
        gl_vao.enable_attribute(gl::shader_spec::aVertex);
    }

};

template<typename vertex_buffer_type>
using gl_mesh = gl_mesh_t<vertex_buffer_type, void>;
using dynamic_gl_mesh = gl_mesh<gl::basic_buffer>;
using static_gl_mesh = gl_mesh<gl::storage_buffer>;



template<typename vertex_buffer_type, typename indecie_buffer_type>
requires(std::is_void_v<indecie_buffer_type> == false)
struct gl_mesh_t<vertex_buffer_type, indecie_buffer_type> : gl_mesh_t<vertex_buffer_type, void>{
    using _gl_mesh_t = gl_mesh_t<vertex_buffer_type, void>;
    inline static constexpr gl::enums::buffer::type indecie_type = gl::enums::buffer::ELEMENT_ARRAY_BUFFER;
    indecie_buffer_type     gl_indecies;

    void draw(gl::enums::drawmode dm, GLint beg, size_t indecie_offset){
        _gl_mesh_t::gl_vao.bind();
        glDrawElements(dm, beg, GL_UNSIGNED_INT, reinterpret_cast<void*>(indecie_offset));
    }
    void drawInstanced(gl::enums::drawmode dm, GLint beg, GLsizei count, size_t indecie_offset, GLsizei instance_count){
        _gl_mesh_t::gl_vao.bind();
        glDrawElementsInstanced(dm, beg, count, reinterpret_cast<void*>(indecie_offset), instance_count);
    }
};

using default_indecie_buffer = gl::typed_buffer<gl::enums::buffer::ELEMENT_ARRAY_BUFFER>;
template<typename vertex_buffer_type, typename indecie_buffer_type>
using instanced_gl_mesh = gl_mesh_t<vertex_buffer_type, indecie_buffer_type>;

using instanced_dynamic_gl_mesh = instanced_gl_mesh<gl::basic_buffer, default_indecie_buffer>;
using instanced_static_gl_mesh = instanced_gl_mesh<gl::storage_buffer, default_indecie_buffer>;

