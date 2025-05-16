#pragma once





#include "datatypes/swap-vector.hpp"
#include "gl/buffer.hpp"
#include <cstddef>
#include <iostream>
#include <utility>
template<typename ShaderDataT>
struct instancer{
public:
    //using ShaderDataT = uint;
    using vec = swap_vector<ShaderDataT>;

    operator bool()const{return _M_Handle;}

    instancer(std::nullptr_t){}
    instancer(ShaderDataT&& data = ShaderDataT{}):_M_Handle(_S_Data.append(std::move(data))){}
    instancer(const ShaderDataT& data):_M_Handle(_S_Data.append(data)){}

    inline void destroy(){
        _S_Data.remove(_M_Handle);
        _M_Handle.reset();
    }

    inline void create(const ShaderDataT& v){
        if(!_M_Handle)
            _M_Handle = _S_Data.append(v);
        else _S_Modified = _S_Data.set_cmp(_M_Handle, v) || _S_Modified;
    }
    inline void create(ShaderDataT&& v){
        if(!_M_Handle)
            _M_Handle = _S_Data.append(v);
        else _S_Modified = _S_Data.set_cmp(_M_Handle, std::move(v)) || _S_Modified;
    }

    inline static void flag_modified(){_S_Modified = true;}
    inline static bool modified(){return _S_Modified;}
    inline static bool upload(bool override = false){
        if(!_S_Modified && !override) return false;
        if(!_S_GPUData) _S_GPUData.create();
            //if need to 
        if(_S_GPUCapacity < _S_Data.size()){
            _S_GPUData.data(_S_Data.data(), sizeof(ShaderDataT)*_S_Data.capacity(), gl::enums::buffer::STATIC_DRAW);
            _S_GPUCapacity = _S_Data.capacity();
        }else _S_GPUData.subdata(_S_Data.data(), sizeof(ShaderDataT)*_S_Data.size());
        return true;
    }

    inline vec::handle release(){
        auto out = _M_Handle;
        _M_Handle.reset();
        return out;
    }



    inline static vec& buffer(){return _S_Data;}
    inline static vec::size_type    buffer_size(){return _S_Data.size();}
    inline static vec::size_type    buffer_capacity(){return _S_Data.capacity();}

    inline static gl::basic_buffer& gpu_buffer(){return _S_GPUData;}
    inline static vec::size_type    gpu_buffer_capacity(){return _S_GPUCapacity;}

    inline ShaderDataT get() const{return _S_Data.get(_M_Handle);}
    inline ShaderDataT& get_ref(){return _S_Data.get(_M_Handle);}
    inline const ShaderDataT& get_ref() const{return _S_Data.get(_M_Handle);}
        //alias to create to not freak out the hoes
    inline void set(const ShaderDataT& v){create(v);}


    friend std::ostream& operator<<(std::ostream& os, const instancer& e){
        return os << "instancer<" << typeid(ShaderDataT).name() << ">[" << e._M_Handle << "]{" << e.get_ref() << "}";
    }
protected:
    vec::handle _M_Handle;

    inline static vec               _S_Data;
    inline static gl::basic_buffer  _S_GPUData{nullptr};
    inline static vec::size_type    _S_GPUCapacity = 0;
    inline static bool              _S_Modified = true;
};