#pragma once
#include <span>
#include <tuple>
#pragma once





#include "datatypes/multi-swap-vector.hpp"
#include "gl/buffer.hpp"
#include <cstddef>
#include <iostream>
#include <utility>
template<typename ...Ts>
struct instancer{
public:
    //using ShaderDataT = uint;
    using vec = multi_swap_vector<Ts...>;

    operator bool()const{return _M_Handle;}

    instancer(std::nullptr_t){}
    instancer(vec::val_tuple&&          data):_M_Handle(_S_Data.append(std::move(data))){}
    instancer(const vec::val_tuple&     data):_M_Handle(_S_Data.append(data)){}
    instancer(vec::ref_tuple&           data):_M_Handle(_S_Data.append(data)){}
    instancer(const vec::cref_tuple&    data):_M_Handle(_S_Data.append(data)){}

    inline void destroy(){
        _S_Data.remove(_M_Handle);
        _M_Handle.reset();
    }
    inline void create(vec::val_tuple&&       v){
        if(!_M_Handle){
            _M_Handle = _S_Data.append(std::move(v));
            _S_Modified = true;
        }else _S_Modified = _S_Data.set_cmp(_M_Handle, v) || _S_Modified;
    }
    inline void create(const vec::val_tuple&  v){
        if(!_M_Handle){
            _M_Handle = _S_Data.append(v);
            _S_Modified = true;
        }else _S_Modified = _S_Data.set_cmp(_M_Handle, v) || _S_Modified;
    }
    inline void create(vec::ref_tuple&        v){
        if(!_M_Handle){
            _M_Handle = _S_Data.append(v);
            _S_Modified = true;
        }else _S_Modified = _S_Data.set_cmp(_M_Handle, v) || _S_Modified;
    }
    inline void create(const vec::cref_tuple& v){
        if(!_M_Handle){
            _M_Handle = _S_Data.append(v);
            _S_Modified = true;
        }else _S_Modified = _S_Data.set_cmp(_M_Handle, v) || _S_Modified;
    }

    inline static void flag_modified(){_S_Modified = true;}
    inline static bool modified(){return _S_Modified;}
    inline static bool upload(bool override = false){
        if(!_S_Modified && !override) return false;
        if(!_S_GPUData) _S_GPUData.create();
            //if need to 
        if(_S_GPUCapacity < _S_Data.size()){
            _gpu_data();
        }else _gpu_subdata();
        return true;
    }

    inline vec::handle release(){
        auto out = _M_Handle;
        _M_Handle.reset();
        return out;
    }



    inline static vec&              buffer(){return _S_Data;}
    inline static vec::size_type    buffer_size(){return _S_Data.size();}
    inline static vec::size_type    buffer_capacity(){return _S_Data.capacity();}

    inline static std::span<gl::basic_buffer, sizeof...(Ts)> gpu_buffers(){return _S_GPUData;}
    inline static vec::size_type    gpu_buffer_capacity(){return _S_GPUCapacity;}

    //inline auto get() const{return _S_Data.get(_M_Handle);}
    inline auto& get_ref(){return _S_Data.get(_M_Handle);}
    inline const auto& get_ref() const{return _S_Data.get(_M_Handle);}
        //alias to create to not freak out the hoes

    friend std::ostream& operator<<(std::ostream& os, const instancer& e){
        os << "instancer< ";
        ((os << typeid(Ts).name() << ' '),...);
        os << ">[" << e._M_Handle << "]{" << e.get_ref() << "}";
        return os;
    }
protected:
    vec::handle _M_Handle;

    inline static vec               _S_Data;
    inline static gl::basic_buffer  _S_GPUData[sizeof...(Ts)]{nullptr};
    inline static vec::size_type    _S_GPUCapacity = 0;
    inline static bool              _S_Modified = true;

private:
    template<std::size_t ...Is>
    inline static void _gpu_data(const vec::size_type new_capacity = _S_Data.capacity(), std::index_sequence<Is...> = vec::sequence){
        (_S_GPUData.data(_S_Data.template data<Is>(), sizeof(std::tuple_element_t<Is, std::tuple<Ts...>>)*new_capacity, gl::enums::buffer::STATIC_DRAW), ...);
        _S_GPUCapacity = new_capacity;
    }
    template<std::size_t ...Is>
    inline static void _gpu_subdata(std::index_sequence<Is...> = vec::sequence){
        const typename vec::size_type size = _S_Data.size();
        (_S_GPUData.subdata( _S_Data.template data<Is>(), sizeof(std::tuple_element_t<Is, std::tuple<Ts...>>)*size, gl::enums::buffer::STATIC_DRAW), ...);
    }
};