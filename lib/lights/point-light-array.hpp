#pragma once
#include "cubemap/cube-map-array.hpp"
#include "cubemap/shadow-map-array.hpp"
#include "gl/buffer.hpp"
#include "gl/buffer_enums.hpp"
#include "gl/texture_enums.hpp"
#include <array>
#include <cstddef>
#include <glm/ext/matrix_float4x4.hpp>
#include <span>
#include <sstream>
#include <stdexcept>
#include <sys/types.h>



template<typename point_light_type>
struct point_light_array_t{
    inline static constexpr uint npos = point_light_type::npos;
    
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
            if(l.index() != npos) continue;
            l = pl;
            l.index(i);
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
            _M_GPULightData.subdata(_M_LightData, sizeof(*_M_LightData)*_M_ArraySize);
            _M_ModifiedData = false;
        }
        if(_M_ModifiedMatricies){
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
};

