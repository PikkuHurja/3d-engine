#pragma once
#include "camera/camera.hpp"
#include "cubemap/cube-map-array.hpp"
#include "cubemap/shadow-map-array.hpp"
#include "draw/draw.hpp"
#include "gl/buffer.hpp"
#include "gl/buffer_enums.hpp"
#include "gl/draw_enums.hpp"
#include "gl/program.hpp"
#include "gl/texture_enums.hpp"
#include "gl/vertex_array.hpp"
#include "shader/load.hpp"
#include <array>
#include <cstddef>
#include <glm/ext/matrix_float4x4.hpp>
#include <iostream>
#include <opengl-framework/Wrappers/GLEW.hpp>
#include <span>
#include <sstream>
#include <stdexcept>
#include <sys/types.h>


/*
    technically we could reinterpret the _M_GPULightData so, that the vec4, that makes up its position and radius would be passed to a vertex shader,
    and its other vec4 can be used to display the intensity change, but that would require some honest work...
*/

template<typename point_light_type>
struct point_light_array_t{
    inline static constexpr uint npos = point_light_type::npos;

    inline static gl::program _S_DebugProgram{nullptr};

    enum debug_attributes{
        aPositionRadius = 0,
        aColorIntensity = 1,
    };
    
    shadow_map_array            _M_ShadowMapArray       = {nullptr};
    cube_map_array              _M_TintMapArray         = {nullptr};
    uint                        _M_ArraySize            = 0;
    bool                        _M_ModifiedData         = true;
    bool                        _M_ModifiedMatricies    = true;
    point_light_type*           _M_LightData            = nullptr;
    std::array<glm::mat4, 6>*   _M_LightMatricies       = nullptr;

    gl::typed_buffer<gl::enums::buffer::SHADER_STORAGE_BUFFER> _M_GPULightData      {nullptr};
    gl::typed_buffer<gl::enums::buffer::SHADER_STORAGE_BUFFER> _M_GPULightMatricies {nullptr};
    gl::typed_buffer<gl::enums::buffer::SHADER_STORAGE_BUFFER> _M_GPUEnabledLights  {nullptr};


    bool has_shadows()const{return _M_ShadowMapArray;}
    bool has_tints()const{return _M_TintMapArray;}


    bool exists(uint index){
        if(index >= _M_ArraySize) 
            throw std::runtime_error((std::ostringstream() << "point_light_array_t::exists(uint index = " << index << "): index out of range: "<<index<<" >= "<<_M_ArraySize<<"!").str());
        return _M_LightData[index].index();
    }

    std::span<const point_light_type> span()const noexcept{
        return {_M_LightData, _M_ArraySize};
    }
        //we can do packing here, so that the arrays are continous, tough quite overkill in my opinion
    void update(point_light_type& pl){
        uint index = pl.index();
        if(index == npos) return;

        point_light_type& in_gpu = _M_LightData[index];
        if(in_gpu == pl) return;

        _M_ModifiedData = true;

        if(pl.regen_matricies(in_gpu)){
            pl.matricies(_M_LightMatricies[index]);
            _M_ModifiedMatricies = true;
        }

        in_gpu = pl;
    }

    const point_light_type& add(const point_light_type& pl = {}){
        for(size_t i = 0; i < _M_ArraySize; i++){
            point_light_type& l = _M_LightData[i];
            if(l.index() != npos)  //exists
                continue;
            
            l = pl;
            l.index(i);
            _M_ModifiedData = true;
            _M_ModifiedMatricies = true;
            return l;
        }
        throw std::runtime_error("point_light_array_t::add(...): out of space!");
    }
        //move the point light to a new index
    void move(point_light_type& pl, uint index){
        if(pl.index() <= index || exists(index)) return;
        
        uint old_index = pl.index();
        auto new_pl = _M_LightData[index];
        new_pl = pl;
        new_pl.index(index);
        _M_LightData[old_index] = point_light_type{};
        new_pl.matricies(_M_LightMatricies[index]);
        _M_ModifiedData = true;
        _M_ModifiedMatricies = true;

    }

    void upload(){
        if(_M_ModifiedData ){
            std::cout << "Uploading light_subdata...\n";
            _M_GPULightData.subdata(_M_LightData, sizeof(*_M_LightData)*_M_ArraySize);
            _M_ModifiedData = false;
        }
        if(_M_ModifiedMatricies){
            std::cout << "Uploading matrix_subdata...\n";
            _M_GPULightMatricies.subdata(_M_LightMatricies, sizeof(*_M_LightMatricies)*_M_ArraySize);
            _M_ModifiedMatricies = false;
        }
    }


    void destroy(){
        _M_ShadowMapArray.texture().destroy();
        _M_TintMapArray.texture().destroy();
        _M_ArraySize = 0;
        _M_GPULightData.destroy();
        _M_ModifiedData = false;
        _M_GPULightMatricies.destroy();
        _M_ModifiedMatricies = false;
        delete [] _M_LightData;
        _M_LightData = nullptr;
    }

    void create(
        const uint count, 
        const uint shadow_resolution=0, 
        const uint tint_resolution=0, 
        const gl::enums::texture::format_storage &shadow_storage_type = gl::enums::texture::STORAGE_DEPTH_COMPONENT24, 
        const gl::enums::texture::format_storage &tint_storage_type = gl::enums::texture::STORAGE_RGB8
    ){
        if(!count) return destroy();
        _M_ArraySize = count;

        if(shadow_resolution)   _M_ShadowMapArray.create(shadow_resolution,_M_ArraySize,shadow_storage_type);
        else                    _M_ShadowMapArray.texture().destroy();

        if(tint_resolution)     _M_TintMapArray.create(tint_resolution, _M_ArraySize, tint_storage_type);
        else                    _M_TintMapArray.texture().destroy();

        if(_M_LightData) delete [] _M_LightData;
        _M_LightData = new point_light_type[_M_ArraySize]();

        if(!_M_GPULightData) _M_GPULightData.create();
        _M_GPULightData.data(_M_LightData, sizeof(*_M_LightData)*_M_ArraySize, gl::enums::buffer::usage::DYNAMIC_DRAW);

        if(!_M_GPULightMatricies) _M_GPULightMatricies.create();
        _M_GPULightMatricies.data(_M_LightMatricies, sizeof(*_M_LightMatricies)*_M_ArraySize, gl::enums::buffer::usage::DYNAMIC_DRAW);
        
        _M_ModifiedData         = false;
        _M_ModifiedMatricies    = false;
    }

    point_light_array_t(std::nullptr_t = nullptr){}

        //create
    point_light_array_t(
        const uint count, 
        const uint shadow_resolution=0, 
        const uint tint_resolution=0, 
        const gl::enums::texture::format_storage &shadow_storage_type = gl::enums::texture::STORAGE_DEPTH_COMPONENT24, 
        const gl::enums::texture::format_storage &tint_storage_type = gl::enums::texture::STORAGE_RGB8
    ){
        create(count,shadow_resolution,tint_resolution,shadow_storage_type,tint_storage_type);
    }


    bool debug_init(){
        return (_S_DebugProgram = shader::load("ass/shaders/light-point-display"));
    }
        //returns true if error occured (vao can be in an invalid state)
    bool debug_bind(gl::vertex_array& debug_vao){
        debug_vao.bind();
        debug_vao.enable_attribute(aPositionRadius);  //pos, rad
        debug_vao.enable_attribute(aColorIntensity);  //color, intensity
        _M_GPULightData.bind(gl::enums::buffer::type::ARRAY_BUFFER);
        _M_GPULightData.attribute(aPositionRadius, 4, gl::enums::buffer::FLOAT, sizeof(point_light_type), false, point_light_type::position_radius_offset);
        _M_GPULightData.attribute(aColorIntensity, 4, gl::enums::buffer::FLOAT, sizeof(point_light_type), false, point_light_type::color_intensity_offset);
        debug_vao.unbind();
        _M_GPULightData.unbind(gl::enums::buffer::ARRAY_BUFFER);
        return false;
    }

    void debug_draw(gl::vertex_array& debug_vao, camera_t& camera){
        if(!_S_DebugProgram)
            throw std::runtime_error("point_light_array_t<...>::debug_draw: _S_DebugProgram was uninitialized / failed to initialize!\n");

        _S_DebugProgram.use();
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glDisable(GL_BLEND);
        camera.bind();
        debug_vao.bind();
        gl::draw_one(gl::enums::POINTS, 0, _M_ArraySize);
    }
};

