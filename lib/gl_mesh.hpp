#pragma once

#include "gl/buffer.hpp"
#include "gl/buffer_enums.hpp"
#include "gl/draw_enums.hpp"
#include "gl/shader_spec.hpp"
#include "gl/vertex_array.hpp"

#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/glm.hpp>
#include <sys/types.h>


template<typename T>
concept vertex_like = std::is_same_v<std::remove_all_extents_t<T>, float> || std::is_same_v<std::remove_all_extents_t<T>, glm::vec2> || std::is_same_v<std::remove_all_extents_t<T>, glm::vec3>;

template<vertex_like T>
inline static constexpr int vertex_dimention(){
    return sizeof(T)/sizeof(float);
}


template<
    bool _has_verticies,
    bool _has_texturemaps,
    bool _has_normals,
    bool _has_tangents,
    bool _has_bitangents,
    bool _has_indecies,
    bool _stores_vertex_count = true
>
struct gl_mesh_t{
    inline static constexpr bool has_verticies(){return _has_verticies;}
    inline static constexpr bool has_texturemaps(){return _has_texturemaps;}
    inline static constexpr bool has_normals(){return _has_normals;}
    inline static constexpr bool has_tangents(){return _has_tangents;}
    inline static constexpr bool has_bitangents(){return _has_bitangents;}
    inline static constexpr bool has_indecies(){return _has_verticies && _has_indecies;}
    inline static constexpr bool stores_vertex_count(){return _stores_vertex_count;}

    gl::vertex_array gl_vao;
    gl::typed_buffer<gl::enums::buffer::ARRAY_BUFFER>           a_gl_verticies  [_has_verticies]                 {};
    gl::typed_buffer<gl::enums::buffer::ARRAY_BUFFER>           a_gl_texturemap [_has_texturemaps]               {};
    gl::typed_buffer<gl::enums::buffer::ARRAY_BUFFER>           a_gl_normals    [_has_normals]                   {};
    gl::typed_buffer<gl::enums::buffer::ARRAY_BUFFER>           a_gl_tangents   [_has_tangents]                  {};
    gl::typed_buffer<gl::enums::buffer::ARRAY_BUFFER>           a_gl_bitangents [_has_bitangents]                {};
    gl::typed_buffer<gl::enums::buffer::ELEMENT_ARRAY_BUFFER>   a_gl_indecies   [_has_verticies*_has_indecies]   {};
    uint                                                        v_vertex_count  [_stores_vertex_count];
    const uint& vertex_count()const requires(stores_vertex_count()){
        return *v_vertex_count;
    }

    void draw(gl::enums::drawmode dm, GLint beg, GLsizei count) requires(!has_indecies()) {
        gl_vao.bind();
        glDrawArrays(dm, beg, count);
    }
    void draw(gl::enums::drawmode dm, GLint count, size_t indecie_offset = 0) requires(has_indecies()) {
        gl_vao.bind();
        glDrawElements(dm, count, GL_UNSIGNED_INT, reinterpret_cast<void*>(indecie_offset));
    }
    void drawInstanced(gl::enums::drawmode dm, GLint beg, GLsizei count, GLsizei instance_count) requires(!has_indecies()) {
        gl_vao.bind();
        glDrawArraysInstanced(dm, beg, count, instance_count);
    }
    void drawInstanced(gl::enums::drawmode dm, GLsizei count, GLsizei instance_count, size_t indecie_offset = 0) requires(has_indecies()) {
        gl_vao.bind();
        glDrawElementsInstanced(dm, count, GL_UNSIGNED_INT, reinterpret_cast<void*>(indecie_offset), instance_count);
    }


    void draw(gl::enums::drawmode dm) requires(!has_indecies() && stores_vertex_count()) {
        gl_vao.bind();
        glDrawArrays(dm, vertex_count());
    }
    void draw(gl::enums::drawmode dm, size_t indecie_offset = 0) requires(has_indecies() && stores_vertex_count()) {
        gl_vao.bind();
        glDrawElements(dm, vertex_count(), GL_UNSIGNED_INT, reinterpret_cast<void*>(indecie_offset));
    }
    void drawInstanced(gl::enums::drawmode dm, GLsizei instance_count) requires(!has_indecies() && stores_vertex_count()) {
        gl_vao.bind();
        glDrawArraysInstanced(dm, 0, vertex_count(), instance_count);
    }
    void drawInstanced(gl::enums::drawmode dm,  GLsizei instance_count, size_t indecie_offset = 0) requires(has_indecies() && stores_vertex_count()) {
        gl_vao.bind();
        glDrawElementsInstanced(dm, vertex_count(), GL_UNSIGNED_INT, reinterpret_cast<void*>(indecie_offset), instance_count);
    }



        //soo much data :3
    void create(
        const uint                          count,
        const uint*                const    indecies = nullptr,
        const vertex_like auto*    const    verticies = nullptr,
        const glm::vec2*           const    texturemaps = nullptr,
        const glm::vec3*           const    normals = nullptr,
        const glm::vec3*           const    tangents = nullptr,
        const glm::vec3*           const    bitangents = nullptr
    ){
        gl_vao.bind();

        if constexpr (has_verticies())      {
            gl_vao.enable_attribute (gl::shader_spec::aVertex);
            a_gl_verticies->bind();
            a_gl_verticies->data(verticies, count*sizeof(*verticies), gl::enums::buffer::usage::STATIC_DRAW);
            a_gl_verticies->attribute(gl::shader_spec::aVertex, sizeof(*verticies)/sizeof(float), gl::enums::buffer::FLOAT);

        }else{ gl_vao.disable_attribute(gl::shader_spec::aVertex); }
        if constexpr (has_texturemaps())    {
            gl_vao.enable_attribute (gl::shader_spec::aUV);
            a_gl_texturemap->bind();
            a_gl_texturemap->data(texturemaps, count*sizeof(*texturemaps), gl::enums::buffer::usage::STATIC_DRAW);
            a_gl_texturemap->attribute(gl::shader_spec::aUV, sizeof(*texturemaps)/sizeof(float), gl::enums::buffer::FLOAT);

        }else{ gl_vao.disable_attribute(gl::shader_spec::aUV); }
        if constexpr (has_normals())        {
            gl_vao.enable_attribute (gl::shader_spec::aNormal);
            a_gl_normals->bind();
            a_gl_normals->data(normals, count*sizeof(*normals), gl::enums::buffer::usage::STATIC_DRAW);
            a_gl_normals->attribute(gl::shader_spec::aNormal, sizeof(*normals)/sizeof(float), gl::enums::buffer::FLOAT);

        }else{ gl_vao.disable_attribute(gl::shader_spec::aNormal); }
        if constexpr (has_tangents())       {
            gl_vao.enable_attribute (gl::shader_spec::aTangent);
            a_gl_tangents->bind();
            a_gl_tangents->data(tangents, count*sizeof(*tangents), gl::enums::buffer::usage::STATIC_DRAW);
            a_gl_tangents->attribute(gl::shader_spec::aTangent, sizeof(*tangents)/sizeof(float), gl::enums::buffer::FLOAT);

        }else{ gl_vao.disable_attribute(gl::shader_spec::aTangent); }
        if constexpr (has_bitangents())     {
            gl_vao.enable_attribute (gl::shader_spec::aBitangent);
            a_gl_bitangents->bind();
            a_gl_bitangents->data(bitangents, count*sizeof(*bitangents), gl::enums::buffer::usage::STATIC_DRAW);
            a_gl_bitangents->attribute(gl::shader_spec::aBitangent, sizeof(*bitangents)/sizeof(float), gl::enums::buffer::FLOAT);

        }else{ gl_vao.disable_attribute(gl::shader_spec::aBitangent); }
        if constexpr(has_indecies()){
            a_gl_indecies->bind();
            a_gl_indecies->data(indecies, count*sizeof(*indecies), gl::enums::buffer::STATIC_DRAW);
        }
        if constexpr(stores_vertex_count()){
            *v_vertex_count = count;
        }
        gl_vao.unbind();
    
    
    }


};
