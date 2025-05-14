#pragma once
#include <algorithm>
#include <cstddef>
#include <memory>
#include <sys/types.h>
#include <utility>



template<typename T>
class sparse_vector{
public:
    using size_type = uint;
    struct handle{
        size_type _M_Key;
        T    _M_Value;
    };

    handle add(const T& value = T{}){
            //check if some spot is empty
            //make this more efficent by making a bitset or something
        for(size_type i = 0; i < _M_Size; i++) if(!_M_Data[i]){
            _M_Data[i] = value;
            return handle{i, value};
        }
            //push back 
        if(_M_Size >= _M_Capasity){
            //realloc
            size_type new_capasity = _M_Capasity < 8? 16 : _M_Capasity * 2;
            std::unique_ptr<T[]> new_data = std::make_unique<T[]>(new_capasity);
            std::move(_M_Data.get(), _M_Data.get() + _M_Size, new_data.get());
            _M_Data = std::move(new_data);
        }
        _M_Data[_M_Size] = value;
        return handle{_M_Size++, value};
    }

    void set(const handle& h)               {_M_Data[h._M_Key] = h._M_Value;}
    void rm(const uint& index)              {_M_Data[index] = T{};}
    void rm(const handle& handle)           {rm(handle._M_Key);}
        //will use the value that is stored in the vector
    void mv(const handle& value, uint index){
        _M_Data[index] = std::move(_M_Data[value._M_Key]);
        _M_Data[value._M_Key] = T{};
    }

    handle get(const uint& index)   const   {return {index, _M_Data[index]};}
    void fetch(handle& h)           const   {h._M_Value = _M_Data[h._M_Key];}



    sparse_vector(std::nullptr_t = nullptr){}
    sparse_vector(uint capasity):_M_Capasity(capasity),_M_Data(std::make_unique<T[]>(capasity)){}

        //uniqptr, so auto deletor
    ~sparse_vector(){}
private:
    size_type _M_Size        = 0;
    size_type _M_Capasity    = 0;
    std::unique_ptr<T[]>   _M_Data;
};