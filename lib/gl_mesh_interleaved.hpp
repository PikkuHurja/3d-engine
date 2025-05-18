#pragma once


#include "gl/buffer.hpp"
#include "gl/buffer_enums.hpp"
#include "gl/shader_spec.hpp"
#include "gl/vertex_array.hpp"
#include "gl_mesh.hpp"
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <glm/detail/qualifier.hpp>
#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float3.hpp>
#include <initializer_list>
#include <iostream>
#include <memory>
#include <sys/types.h>
#include <type_traits>


template<
    uint _dimention_verticies = 3,  // 0 to disable them
    bool _has_uvs = false,  // 2 d
    bool _has_normals = false,      // 3 d
    bool _has_tangents = false,     // 3 d
    bool _has_bitangents = false    // 3 d
>
struct gl_mesh_interleaved_t{
    using vertex_t = std::conditional_t<_dimention_verticies!=0, glm::vec<_dimention_verticies, float>, void>;
    using texture_map_t = glm::vec2;
    using normal_t = glm::vec3;
    using tangent_t = glm::vec3;
    using bitangent_t = glm::vec3;

    inline static constexpr size_t  vertex_attrib_size()        {return _dimention_verticies;}
    inline static constexpr size_t  uv_attrib_size()    {return 2;}
    inline static constexpr size_t  normals_attrib_size()       {return 3;}
    inline static constexpr size_t  tangents_attrib_size()      {return 3;}
    inline static constexpr size_t  bitangents_attrib_size()    {return 3;}

    inline static constexpr size_t vertex_size(){
        return sizeof(float) * _dimention_verticies;
    }
    inline static constexpr size_t uv_size(){
        return sizeof(glm::vec<uv_attrib_size(), float>) * _has_uvs;
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
    inline static constexpr size_t uv_offset(){
        return _has_uvs ? vertex_size() : -1;
    }
    inline static constexpr size_t normals_offset(){
        return _has_normals ? vertex_size()+uv_size() : -1;
    }
    inline static constexpr size_t tangents_offset(){
        return _has_tangents ? vertex_size()+uv_size()+normals_size() : -1;
    }
    inline static constexpr size_t bitangents_offset(){
        return _has_bitangents ? vertex_size()+uv_size()+normals_size()+tangents_size() : -1;
    }

    inline static constexpr size_t per_vertex_size(){
        return 
            vertex_size()+ 
            uv_size()+
            normals_size()+
            tangents_size()+
            bitangents_size();
    }

    uint vertex_count()const{return v_vertex_count;}

    gl::vertex_array                                            gl_vao          {nullptr};
    gl::typed_buffer<gl::enums::buffer::ARRAY_BUFFER>           gl_data         {nullptr};
    gl::typed_buffer<gl::enums::buffer::ELEMENT_ARRAY_BUFFER>   gl_indecies     {nullptr};
    uint                                                        v_vertex_count  = 0;
    uint                                                        v_indecie_count = 0;

    void bind()     {gl_vao.bind();}
    inline static void unbind()   {gl::vertex_array::unbind();}

    inline void create(uint vertex_count, uint indecie_count, void* vertex_data = nullptr, uint* indecie_data = nullptr){
        if(!gl_vao) gl_vao.create();
        gl_vao.bind();

        if (indecie_count && indecie_data){
            if(!gl_indecies) gl_indecies.create();
            
            gl_indecies.bind();
            gl_indecies.data(indecie_data, indecie_count*sizeof(uint), gl::enums::buffer::STATIC_DRAW);
            v_indecie_count = indecie_count;
        }

        if(!gl_data) gl_data.create();

        gl_data.bind();
        gl_data.data(vertex_data, per_vertex_size()*vertex_count, gl::enums::buffer::STATIC_DRAW);

        v_vertex_count = vertex_count;

        if constexpr(_dimention_verticies){
            gl_vao.enable_attribute(gl::shader_spec::aVertex);
            gl_data.attribute(gl::shader_spec::aVertex, vertex_attrib_size(), gl::enums::buffer::FLOAT, per_vertex_size(), GL_FALSE, vertex_offset());        
        }else gl_vao.disable_attribute(gl::shader_spec::aVertex);
        
        if constexpr(_has_uvs){
            gl_vao.enable_attribute(gl::shader_spec::aUV);
            gl_data.attribute(gl::shader_spec::aUV, uv_attrib_size(), gl::enums::buffer::FLOAT, per_vertex_size(), GL_FALSE, uv_size());
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
        from planar, 
        use nullptr for unset
    */
    inline void create(
        uint vertex_count,
        vertex_t* verticies,
        texture_map_t* uv,
        normal_t* normals,
        tangent_t* tangents,
        bitangent_t* bitangents,
        uint indecie_count,
        uint* indecie_data
    ){
        std::unique_ptr<uint8_t[]> vertex_data = std::make_unique<uint8_t[]>(per_vertex_size()*vertex_count);
        for(size_t i = 0; i < vertex_count; i++){
            uint8_t* data = vertex_data.get() + i*per_vertex_size();
            if constexpr(_dimention_verticies)    if(verticies)      { std::memmove(data+vertex_offset(),     verticies+i,  vertex_size());     }
            if constexpr(_has_uvs)        if(uv)     { std::memmove(data+uv_offset(), uv+i, uv_size()); }
            if constexpr(_has_normals)            if(normals)        { std::memmove(data+normals_offset(),    normals+i,    normals_size());    }
            if constexpr(_has_tangents)           if(tangents)       { std::memmove(data+tangents_offset(),   tangents+i,   tangents_size());   }
            if constexpr(_has_bitangents)         if(bitangents)     { std::memmove(data+bitangents_offset(), bitangents+i, bitangents_size()); }
        }

        create(vertex_count, indecie_count, vertex_data.get(), indecie_data);
    }



};

struct rt_gl_mesh_interleaved_t{
    using vertex_t =        glm::vec3;
    using texture_map_t =   glm::vec2;
    using normal_t =        glm::vec3;
    using tangent_t =       glm::vec3;
    using bitangent_t =     glm::vec3;

    inline static constexpr size_t  vertex_attrib_size()        {return 3;}
    inline static constexpr size_t  uv_attrib_size()    {return 2;}
    inline static constexpr size_t  normals_attrib_size()       {return 3;}
    inline static constexpr size_t  tangents_attrib_size()      {return 3;}
    inline static constexpr size_t  bitangents_attrib_size()    {return 3;}

    inline size_t vertex_size() const{
        return (sizeof(float) * v_vertex_size) * has_verticies();
    }
    inline size_t uv_size() const{
        return (sizeof(glm::vec<uv_attrib_size(), float>)) * has_uv();
    }
    inline size_t normals_size() const{
        return (sizeof(glm::vec<normals_attrib_size(), float>)) * has_normals();
    }
    inline size_t tangents_size() const{
        return (sizeof(glm::vec<tangents_attrib_size(), float>)) * has_tangents();
    }
    inline size_t bitangents_size() const{
        return (sizeof(glm::vec<bitangents_attrib_size(), float>)) * has_bitangents();
    }

    inline size_t vertex_offset() const{
        return has_verticies() ? 0 : -1;
    }
    inline size_t uv_offset() const{
        return has_uv() ? vertex_size() : -1;
    }
    inline size_t normals_offset() const{
        return has_normals() ? vertex_size()+uv_size() : -1;
    }
    inline size_t tangents_offset() const{
        return has_tangents() ? vertex_size()+uv_size()+normals_size() : -1;
    }
    inline size_t bitangents_offset() const{
        return has_bitangents() ? vertex_size()+uv_size()+normals_size()+tangents_size() : -1;
    }

    bool has_verticies()const      {return v_enabled_attributes & 1<<gl::shader_spec::aVertex;}
    bool has_uv()const             {return v_enabled_attributes & 1<<gl::shader_spec::aUV;}
    bool has_normals()const        {return v_enabled_attributes & 1<<gl::shader_spec::aNormal;}
    bool has_tangents()const       {return v_enabled_attributes & 1<<gl::shader_spec::aTangent;}
    bool has_bitangents()const     {return v_enabled_attributes & 1<<gl::shader_spec::aBitangent;}
    bool has_indecies()const       {return v_indecie_count;}

    inline size_t per_vertex_size() const{
        return vertex_size()+uv_size()+normals_size()+tangents_size()+bitangents_size();
    }

    uint vertex_count()const{return v_vertex_count;}
    gl::vertex_array                                            gl_vao                  {nullptr};
    gl::typed_buffer<gl::enums::buffer::ARRAY_BUFFER>           gl_data                 {nullptr};
    gl::typed_buffer<gl::enums::buffer::ELEMENT_ARRAY_BUFFER>   gl_indecies             {nullptr};
    uint                                                        v_vertex_count          = 0;
    uint                                                        v_indecie_count         = 0;
    uint                                                        v_vertex_size           = 3;

    uint                                                        v_enabled_attributes    = 0;

    void bind()     {gl_vao.bind();}
    inline static void unbind()   {gl::vertex_array::unbind();}


    /*
    inline void create(uint vertex_count, uint indecie_count, void* vertex_data = nullptr, uint* indecie_data = nullptr){
        if(!gl_vao) gl_vao.create();
        gl_vao.bind();

        if (indecie_count && indecie_data){
            if(!gl_indecies) gl_indecies.create();
            
            gl_indecies.bind();
            gl_indecies.data(indecie_data, indecie_count*sizeof(uint), gl::enums::buffer::STATIC_DRAW);
            v_indecie_count = indecie_count;
        }

        if(!gl_data) gl_data.create();

        gl_data.bind();
        gl_data.data(vertex_data, per_vertex_size()*vertex_count, gl::enums::buffer::STATIC_DRAW);

        v_vertex_count = vertex_count;

        if constexpr(_dimention_verticies){
            gl_vao.enable_attribute(gl::shader_spec::aVertex);
            gl_data.attribute(gl::shader_spec::aVertex, vertex_attrib_size(), gl::enums::buffer::FLOAT, per_vertex_size(), GL_FALSE, vertex_offset());        
        }else gl_vao.disable_attribute(gl::shader_spec::aVertex);
        
        if constexpr(_has_uvs){
            gl_vao.enable_attribute(gl::shader_spec::aUV);
            gl_data.attribute(gl::shader_spec::aUV, uv_attrib_size(), gl::enums::buffer::FLOAT, per_vertex_size(), GL_FALSE, uv_size());
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
    inline void create(
        uint vertex_count,
        vertex_t* verticies,
        texture_map_t* uv,
        normal_t* normals,
        tangent_t* tangents,
        bitangent_t* bitangents,
        uint indecie_count,
        uint* indecie_data
    ){
        std::unique_ptr<uint8_t[]> vertex_data = std::make_unique<uint8_t[]>(per_vertex_size()*vertex_count);
        for(size_t i = 0; i < vertex_count; i++){
            uint8_t* data = vertex_data.get() + i*per_vertex_size();
            if constexpr(_dimention_verticies)    if(verticies)      { std::memmove(data+vertex_offset(),     verticies+i,  vertex_size());     }
            if constexpr(_has_uvs)        if(uv)     { std::memmove(data+uv_offset(), uv+i, uv_size()); }
            if constexpr(_has_normals)            if(normals)        { std::memmove(data+normals_offset(),    normals+i,    normals_size());    }
            if constexpr(_has_tangents)           if(tangents)       { std::memmove(data+tangents_offset(),   tangents+i,   tangents_size());   }
            if constexpr(_has_bitangents)         if(bitangents)     { std::memmove(data+bitangents_offset(), bitangents+i, bitangents_size()); }
        }

        create(vertex_count, indecie_count, vertex_data.get(), indecie_data);
    }
    */

    inline uint attribute_size_by_index(uint index){
        switch (index) {
            case 0: return v_vertex_size;
            case 1: return uv_size();
            case 2: return normals_size();
            case 3: return tangents_size();
            case 4: return bitangents_size();
            default: return 0;
        }
    }
    inline uint attribute_offset_by_index(uint index){
        switch (index) {
            case 0: return vertex_offset();
            case 1: return uv_offset();
            case 2: return normals_offset();
            case 3: return tangents_offset();
            case 4: return bitangents_offset();
            default: return 0;
        }
    }
    inline void set_vertex_size(uint size = 3){v_vertex_size = size;}
    template<typename ...Attribs>
    requires (std::is_same_v<gl::shader_spec::VertexInputs, std::remove_all_extents_t<Attribs>> && ...)
    inline void enable_attribute(Attribs... attrib){v_enabled_attributes |= ((1<<static_cast<uint>(attrib)) | ...);}
        //destroy indecies
    inline void destroy_indecies(){
        gl_indecies.destroy();
        v_indecie_count = 0;
    }
    inline void  indecies(const uint indecie_count, const uint* indecie_data, const gl::enums::buffer::usage indecie_usage = gl::enums::buffer::STATIC_DRAW){
        if (!indecie_count)
            return;
        
        if(!gl_indecies) gl_indecies.create();
        
        gl_indecies.bind();
        gl_indecies.data(indecie_data, indecie_count*sizeof(*indecie_data), gl::enums::buffer::STATIC_DRAW);
        v_indecie_count = indecie_count;
    }
    inline void enable_attributes(){
        const uint stride = per_vertex_size();
        uint index = 0;
        uint to_enable = v_enabled_attributes;
        while (to_enable) {
            if(to_enable & 1){
                gl_vao.enable_attribute(index);
                gl_data.attribute(gl::shader_spec::aVertex, attribute_size_by_index(index), gl::enums::buffer::FLOAT, stride, GL_FALSE, attribute_offset_by_index(index));
            }else{
                gl_vao.disable_attribute(index);
            }
            index++;
            to_enable>>=1;
        }
    }
        //enable correct attributes before creating, indecies can be added before or after the fact
    inline void create(
        const uint vertex_count, const void* vertex_data, const gl::enums::buffer::usage vertex_usage = gl::enums::buffer::STATIC_DRAW, 
        const uint indecie_count = 0, const uint* indecie_data = nullptr, const gl::enums::buffer::usage indecie_usage = gl::enums::buffer::STATIC_DRAW
    ){
        if(!gl_vao) gl_vao.create();
        gl_vao.bind();

        indecies(indecie_count, indecie_data);

        uint stride = per_vertex_size();
        if(!gl_data) gl_data.create();
        gl_data.data(vertex_data, stride*vertex_count, gl::enums::buffer::STATIC_DRAW);
        enable_attributes();

        gl_vao.unbind();
        gl_data.unbind();
    }

    inline void create(
        const uint vertex_count,
        const void* verticies, //specify vertex depth beforehand
        const void* uv,
        const void* normals,
        const void* tangents,
        const void* bitangents,
        const gl::enums::buffer::usage vertex_usage = gl::enums::buffer::STATIC_DRAW, 
        const uint indecie_count = 0,
        const uint* indecie_data = nullptr,
        const gl::enums::buffer::usage indecie_usage = gl::enums::buffer::STATIC_DRAW
    ){
        uint stride = per_vertex_size();
        std::unique_ptr<uint8_t[]> vertex_data = std::make_unique<uint8_t[]>(stride*vertex_count);

        if(has_verticies())     assert(verticies);
        if(has_uv())            assert(uv);
        if(has_normals())       assert(normals);
        if(has_tangents())      assert(tangents);
        if(has_bitangents())    assert(bitangents);

        std::cout << "interweaving data...\n";
        for(size_t vtx = 0; vtx < vertex_count; vtx++){
            auto data = vertex_data.get()+vtx*stride;
            if(has_verticies())       { std::memmove(data+vertex_offset(),        reinterpret_cast<const uint8_t*>(verticies)+vtx*vertex_size(),    vertex_size());     }
            if(has_uv())              { std::memmove(data+uv_offset(),            reinterpret_cast<const texture_map_t*>(uv)+vtx,                   uv_size()); }
            if(has_normals())         { std::memmove(data+normals_offset(),       reinterpret_cast<const normal_t*>(normals)+vtx,                   normals_size());    }
            if(has_tangents())        { std::memmove(data+tangents_offset(),      reinterpret_cast<const tangent_t*>(tangents)+vtx,                 tangents_size());   }
            if(has_bitangents())      { std::memmove(data+bitangents_offset(),    reinterpret_cast<const bitangent_t*>(bitangents)+vtx,             bitangents_size()); }
        }

        create(vertex_count, vertex_data.get(), vertex_usage, indecie_count,  indecie_data, indecie_usage);
    }

};