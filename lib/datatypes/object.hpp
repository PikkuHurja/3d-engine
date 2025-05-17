#pragma once

#include "datatypes/multi-instancer.hpp"
#include "draw/draw.hpp"
#include "gl/buffer.hpp"
#include "gl/buffer_enums.hpp"
#include "gl/draw_enums.hpp"
#include "gl/shader_spec.hpp"
#include "gl_mesh_interleaved.hpp"
#include <glm/ext/matrix_float4x4.hpp>
#include "gl/.hpp"

template<typename ...AdditionalGPUData>
struct object{
    inline static rt_gl_mesh_interleaved_t                      _S_Mesh;
    multi_instancer<glm::mat4, uint, AdditionalGPUData...>      _M_Instancer;

    inline static gl::basic_buffer                              _S_CullData;
    inline static gl::basic_buffer                              _S_CullCounter;

    gl::basic_buffer& model_matricies()const{return _M_Instancer.gpu_buffer(0);}
    gl::basic_buffer& instance_mapping()const{return _M_Instancer.gpu_buffer(1);}

        //no culling, no nothing
    void draw_all(){
        _S_Mesh.bind();
        _M_Instancer.gpu_buffer(0).bind_base(gl::enums::buffer::SHADER_STORAGE_BUFFER, gl::shader_spec::ShaderStorageBuffers::ssbModelMatrix);
        _M_Instancer.gpu_buffer(1).bind_base(gl::enums::buffer::SHADER_STORAGE_BUFFER, gl::shader_spec::ShaderStorageBuffers::ssbInstanceMapping);
        gl::draw_indecies_instanced(gl::enums::TRIANGLES, _S_Mesh.v_indecie_count, _M_Instancer.buffer_size());
    }

    void cull_all(){
        _S_CullData.bind_base(gl::enums::buffer::SHADER_STORAGE_BUFFER, 0);
        _S_CullCounter.bind_base(gl::enums::buffer::type::SHADER_STORAGE_BUFFER, 2);
        //_M_Instancer.gpu_buffer(0).bind_base(gl::enums::buffer::SHADER_STORAGE_BUFFER, gl::shader_spec::ShaderStorageBuffers::ssbModelMatrix);
        instance_mapping().bind_base(gl::enums::buffer::SHADER_STORAGE_BUFFER, gl::shader_spec::ShaderStorageBuffers::ssbInstanceMapping);
        gl::draw_indecies_instanced(gl::enums::TRIANGLES, _S_Mesh.v_indecie_count, _M_Instancer.buffer_size());
    }
    
    void draw_all_culled(){
        _S_Mesh.bind();
        model_matricies().bind_base(gl::enums::buffer::SHADER_STORAGE_BUFFER, gl::shader_spec::ShaderStorageBuffers::ssbModelMatrix);
        instance_mapping().bind_base(gl::enums::buffer::SHADER_STORAGE_BUFFER, gl::shader_spec::ShaderStorageBuffers::ssbInstanceMapping);
        gl::draw_indecies_instanced(gl::enums::TRIANGLES, _S_Mesh.v_indecie_count, _M_Instancer.buffer_size());
    }

};