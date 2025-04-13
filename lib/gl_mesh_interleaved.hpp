#pragma once


#include "gl/buffer.hpp"
#include "gl/buffer_enums.hpp"
#include "gl/shader_spec.hpp"
#include "gl/vertex_array.hpp"
#include "gl_mesh.hpp"
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <glm/detail/qualifier.hpp>
#include <glm/ext/vector_float2.hpp>
#include <memory>
#include <sys/types.h>
#include <type_traits>


template<
    uint _dimention_verticies = 3,  // 0 to disable them
    bool _has_texturemaps = false,  // 2 d
    bool _has_normals = false,      // 3 d
    bool _has_tangents = false,     // 3 d
    bool _has_bitangents = false,   // 3 d
    bool _has_indecies = false,     // seperate, uint
    bool _stores_vertex_count = true
>
struct gl_mesh_interleaved_t{
    using vertex_t = std::conditional_t<_dimention_verticies!=0, glm::vec<_dimention_verticies, float>, void>;
    using texture_map_t = glm::vec2;
    using normal_t = glm::vec3;
    using tangent_t = glm::vec3;
    using bitangent_t = glm::vec3;

    inline static constexpr size_t  vertex_attrib_size()        {return _dimention_verticies;}
    inline static constexpr size_t  texturemap_attrib_size()    {return 2;}
    inline static constexpr size_t  normals_attrib_size()       {return 3;}
    inline static constexpr size_t  tangents_attrib_size()      {return 3;}
    inline static constexpr size_t  bitangents_attrib_size()    {return 3;}

    inline static constexpr size_t vertex_size(){
        return sizeof(float) * _dimention_verticies;
    }
    inline static constexpr size_t texturemap_size(){
        return sizeof(glm::vec<texturemap_attrib_size(), float>) * _has_texturemaps;
    }
    inline static constexpr size_t normals_size(){
        return sizeof(glm::vec<normals_attrib_size(), float>) * _has_normals;
    }
    inline static constexpr size_t tangents_size(){
        return sizeof(glm::vec<tangents_attrib_size(), float>) * _has_tangents;
    }
    inline static constexpr size_t bitangents_size(){
        return sizeof(glm::vec<bitangents_attrib_size(), float>) * _has_bitangents;
    }

    inline static constexpr size_t vertex_offset(){
        return _dimention_verticies ? 0 : -1;
    }
    inline static constexpr size_t texturemap_offset(){
        return _has_texturemaps ? vertex_size() : -1;
    }
    inline static constexpr size_t normals_offset(){
        return _has_normals ? vertex_size()+texturemap_size() : -1;
    }
    inline static constexpr size_t tangents_offset(){
        return _has_tangents ? vertex_size()+texturemap_size()+normals_size() : -1;
    }
    inline static constexpr size_t bitangents_offset(){
        return _has_bitangents ? vertex_size()+texturemap_size()+normals_size()+tangents_size() : -1;
    }

    inline static constexpr size_t per_vertex_size(){
        return 
            vertex_size()+ 
            texturemap_size()+
            normals_size()+
            tangents_size()+
            bitangents_size();
    }

    uint vertex_count()const{return _stores_vertex_count ? *v_vertex_count : -1;}

    gl::vertex_array                                            gl_vao{nullptr};
    gl::typed_buffer<gl::enums::buffer::ARRAY_BUFFER>           gl_data{nullptr};
    gl::typed_buffer<gl::enums::buffer::ELEMENT_ARRAY_BUFFER>   gl_indecies[_has_indecies];
    uint                                                        v_vertex_count[_stores_vertex_count];

    void bind()     {gl_vao.bind();}
    inline static void unbind()   {gl::vertex_array::unbind();}

    inline bool create(uint vertex_count, uint indecie_count, void* vertex_data = nullptr, uint* indecie_data = nullptr){
        if(!gl_vao) gl_vao.create();
        gl_vao.bind();

        if constexpr(_has_indecies){
            if(!*gl_indecies) gl_indecies->create();
            
            gl_indecies->bind();
            gl_indecies->data(indecie_data, sizeof(uint)*indecie_count, gl::enums::buffer::STATIC_DRAW);
        }

        if(!gl_data) gl_data.create();

        gl_data.bind();
        gl_data.data(vertex_data, per_vertex_size()*vertex_count, gl::enums::buffer::STATIC_DRAW);

        if constexpr(_stores_vertex_count)
            *v_vertex_count = vertex_count;

        if constexpr(_dimention_verticies){
            gl_vao.enable_attribute(gl::shader_spec::aVertex);
            gl_data.attribute(gl::shader_spec::aVertex, vertex_attrib_size(), gl::enums::buffer::FLOAT, per_vertex_size(), GL_FALSE, vertex_offset());        
        }else gl_vao.disable_attribute(gl::shader_spec::aVertex);
        
        if constexpr(_has_texturemaps){
            gl_vao.enable_attribute(gl::shader_spec::aUV);
            gl_data.attribute(gl::shader_spec::aUV, texturemap_attrib_size(), gl::enums::buffer::FLOAT, per_vertex_size(), GL_FALSE, texturemap_size());
        }else gl_vao.disable_attribute(gl::shader_spec::aUV);

        if constexpr(_has_normals){
            gl_vao.enable_attribute(gl::shader_spec::aNormal);
            gl_data.attribute(gl::shader_spec::aNormal, normals_attrib_size(), gl::enums::buffer::FLOAT, per_vertex_size(), GL_FALSE, normals_size());
        }else gl_vao.disable_attribute(gl::shader_spec::aNormal);

        if constexpr(_has_tangents){
            gl_vao.enable_attribute(gl::shader_spec::aTangent);
            gl_data.attribute(gl::shader_spec::aTangent, tangents_attrib_size(), gl::enums::buffer::FLOAT, per_vertex_size(), GL_FALSE, tangents_size());
        }else gl_vao.disable_attribute(gl::shader_spec::aTangent);

        if constexpr(_has_bitangents){
            gl_vao.enable_attribute(gl::shader_spec::aBitangent);
            gl_data.attribute(gl::shader_spec::aBitangent, bitangents_attrib_size(), gl::enums::buffer::FLOAT, per_vertex_size(), GL_FALSE, bitangents_size());
        }else gl_vao.disable_attribute(gl::shader_spec::aBitangent);

        gl_vao.unbind();
    }
    /*
        use nullptr for unset
    */
    inline bool create(
        uint vertex_count,
        vertex_t* verticies,
        texture_map_t* texturemap,
        normal_t* normals,
        tangent_t* tangents,
        bitangent_t* bitangents,
        uint indecie_count,
        uint* indecie_data
    ){
        if(!gl_vao) gl_vao.create();
        gl_vao.bind();

        if constexpr(_has_indecies){
            if(!*gl_indecies) gl_indecies->create();
            
            gl_indecies->bind();
            gl_indecies->data(indecie_data, sizeof(uint)*indecie_count, gl::enums::buffer::STATIC_DRAW);
        }

        if(!gl_data) gl_data.create();
        gl_data.bind();

        std::unique_ptr<uint8_t[]> vertex_data = std::make_unique<uint8_t[]>(per_vertex_size()*vertex_count);
        
        if constexpr(_stores_vertex_count)
            *v_vertex_count = vertex_count;
        
        for(size_t i = 0; i < vertex_count; i++){
            uint8_t* data = vertex_data.get() + i*per_vertex_size();
            if constexpr(_dimention_verticies)    if(verticies)      { std::memmove(data+vertex_offset(),     verticies+i,  vertex_size());     }
            if constexpr(_has_texturemaps)        if(texturemap)     { std::memmove(data+texturemap_offset(), texturemap+i, texturemap_size()); }
            if constexpr(_has_normals)            if(normals)        { std::memmove(data+normals_offset(),    normals+i,    normals_size());    }
            if constexpr(_has_tangents)           if(tangents)       { std::memmove(data+tangents_offset(),   tangents+i,   tangents_size());   }
            if constexpr(_has_bitangents)         if(bitangents)     { std::memmove(data+bitangents_offset(), bitangents+i, bitangents_size()); }
        }
    
        gl_data.data(vertex_data.get(), per_vertex_size()*vertex_count, gl::enums::buffer::STATIC_DRAW);
    
        gl_vao.unbind();
    }



};
