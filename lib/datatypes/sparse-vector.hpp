#pragma once
#include <algorithm>
#include <cstddef>
#include <cstring>
#include <memory>
#include <set>
#include <sys/types.h>
#include <utility>


/*
    improper size and capasity management
*/
template<typename T>
class sparse_vector{
public:
    using size_type = uint;
    using bm_type = size_t;
    inline static constexpr size_type bm_bit_count = sizeof(bm_type)*8;
    inline static constexpr size_type bm_bit_round_offset = bm_bit_round_offset-1;
    inline static constexpr size_type npos = -1;


    struct handle{
        size_type _M_Key;
        T    _M_Value;
    };

    handle add(const T& value = T{}){
            //check if some spot is empty
            //make this more efficent by making a bitset or something
        if(size_type p = _next_free(); p != npos){
            _set(p, value);
            return {p, value};
        }
            //push back 
        if(_M_Size >= _M_Capasity){
            _alloc(_M_Capasity < 8? 16 : _M_Capasity * 2);
        }
        _set(_M_Size, value);
        return handle{_M_Size++, value};
    }

    void set(const handle& h)               {_set(h._M_Key, h._M_Value);}
    void rm(const uint& index)              {_unset(index);}
    void rm(const handle& handle)           {rm(handle._M_Key);}
        //will use the value that is stored in the vector
    void mv(const handle& value, uint index){
        _set(index, std::move(_M_Data[value._M_Key]));
        _unset(value._M_Key);
    }

    handle get(const uint& index)   const   {return {index, _M_Data[index]};}
    void fetch(handle& h)           const   {h._M_Value = _M_Data[h._M_Key];}



    sparse_vector(std::nullptr_t = nullptr){}
    sparse_vector(uint capasity){_alloc(capasity);}

        //uniqptr, so auto deletor
    ~sparse_vector(){}
private:

    size_type _next_free(){
        if(_M_Unset.empty())
            return npos;
        size_type unset = *_M_Unset.begin();
        _M_Unset.erase(unset);
        return unset;
    }
    void _alloc(uint new_capasity){
        std::unique_ptr<T[]> new_data = std::make_unique<T[]>(new_capasity);
        std::move(_M_Data.get(), _M_Data.get() + _M_Size, new_data.get());
        
        _M_Data = std::move(new_data);
        _M_Capasity = new_capasity;
    }
    void _set(uint index, const T& v ){
        _M_Data[index] = v;
        _M_Unset.erase(index);
    }
    void _set(uint index, T&& v){
        _M_Data[index] = std::move(v);
        _M_Unset.erase(index);
    }
    void _unset(uint index){
        _M_Data[index] = T{};
        if(index != _M_Size-1)
            _M_Unset.emplace(index);
        else --_M_Size;
    }


    size_type _M_Size       = 0;
    size_type _M_Capasity   = 0;
    std::unique_ptr<T[]>    _M_Data;
    std::set<size_type>     _M_Unset;
};