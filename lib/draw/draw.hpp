#pragma once

#include "gl/draw_enums.hpp"
#include <cstddef>
#include <sys/types.h>
#include <GL/glew.h>

namespace gl {


        // idk if ill ever use these
    template<typename T>
    void draw_one(T& t);
    template<typename T>
    void draw_instanced(T& t);
    template<typename T>
    void draw_indecies(T& t);
    template<typename T>
    void draw_indecies_instanced(T& t);


        // idk if ill ever use these
    inline static void draw_one(::gl::enums::drawmode dm, uint first, uint count){
        glDrawArrays(dm, first, count);
    }

    inline static void draw_instanced(::gl::enums::drawmode dm, uint first, uint vertex_count, uint instance_count){
        glDrawArraysInstanced(dm, first, vertex_count, instance_count);
    }

    inline static void draw_indecies(::gl::enums::drawmode dm, GLsizei indecie_count, GLenum type = GL_UNSIGNED_INT, size_t indecies_offset = 0){
        glDrawElements(dm, indecie_count, type, reinterpret_cast<const void*>(indecies_offset));
    }

    inline static void draw_indecies_instanced(
        ::gl::enums::drawmode dm,
        GLsizei indecie_count,
        GLsizei instance_count,
        GLenum type = GL_UNSIGNED_INT,
        size_t indices_offset = 0
    ){
        glDrawElementsInstanced(dm, indecie_count, type, reinterpret_cast<const void*>(indices_offset), instance_count);
    }

    inline static void multidraw( gl::enums::drawmode dm, const GLint *first, const GLsizei *count, GLsizei num){
        glMultiDrawArrays(dm, first, count, num);
    }

    inline static void multidraw_indecies( gl::enums::drawmode dm, const GLsizei *count, const size_t * const indices, GLsizei num, GLenum type = GL_UNSIGNED_INT){
        glMultiDrawElements(dm, count, type, reinterpret_cast<const void*const*const>(indices), num);
    }


}