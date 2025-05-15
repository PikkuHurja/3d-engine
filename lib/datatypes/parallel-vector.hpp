#pragma once



#include <algorithm>
#include <cstddef>
#include <glm/ext/vector_float2.hpp>
#include <iostream>
#include <memory>
#include <ostream>
#include <sys/types.h>
#include <tuple>
#include <utility>


template<typename ...Parallel>
struct parallel_vector{
public:
    using size_type = uint;
    inline static const auto sequence = std::index_sequence_for<Parallel...>{};
    inline void push_back(const Parallel& ...values){
        _expand();
        _move(_M_Size++, values..., sequence);
    }
    inline void push_back(const std::tuple<Parallel...>& value){
        _expand();
        _move(_M_Size++, value, sequence);
    }
    inline void pop_back(){
        _M_Size--;
        _shrink();
    }
    inline void remove(const size_type index){
        if(index == _M_Size-1) return pop_back();
        _shift_backward(index+1, 1, sequence);
        _M_Size--;
        _shrink();
    }
    inline void emplace(const size_type index, const std::tuple<Parallel...>& value){
        _expand();
        _shift_forward(index, 1, sequence);
        _move(index,value, sequence);
        _M_Size++;
    }


    template<typename T> inline T*          get(){return std::get<std::unique_ptr<T[]>>(_M_Data).get();}
    template<size_t Idx> inline auto*       get(){return std::get<Idx>(_M_Data).get();}

    template<typename T> inline const T*    get()const {return std::get<std::unique_ptr<T[]>>(_M_Data).get();}
    template<size_t Idx> inline const auto* get()const {return std::get<Idx>(_M_Data).get();}

    template<typename T> inline T&          get(const uint index){return *(std::get<std::unique_ptr<T[]>>(_M_Data).get()+index);}
    template<size_t Idx> inline auto&       get(const uint index){return *(std::get<Idx>(_M_Data).get()+index);}
    
    template<typename T> inline const T&    get(const uint index) const{return *(std::get<std::unique_ptr<T[]>>(_M_Data).get()+index);}
    template<size_t Idx> inline const auto& get(const uint index) const{return *(std::get<Idx>(_M_Data).get()+index);}

    inline std::tuple<Parallel*...>         get(const uint index) {return _get(index, sequence);}
    inline std::tuple<const Parallel*...>   get(const uint index) const {return _get(index, sequence);}


    inline size_type size()const{
        return _M_Size;
    }
    inline size_type capacity()const{
        return _M_Capacity;
    }
    inline const std::tuple<std::unique_ptr<Parallel[]>...>& data(){
        return _M_Data;
    }
private:
    template<size_t ...Is>
    inline std::tuple<Parallel*...> _get(const uint index, const std::index_sequence<Is...>) {
        return std::tuple<Parallel*...>{(std::get<Is>(_M_Data).get()+index)...};
    }
    template<size_t ...Is>
    inline std::tuple<const Parallel*...> _get(const uint index, const std::index_sequence<Is...>) const{
        return std::tuple<const Parallel*...>{(std::get<Is>(_M_Data).get()+index)...};
    }

    inline void _expand(){
        if(_M_Size >= _M_Capacity)
            _alloc(_M_Capacity <= 8 ? 16 : _M_Capacity*2);
    }
    inline void _shrink(){
        if(_M_Size/2 < _M_Capacity && _M_Capacity > 32) 
            _alloc(_M_Capacity/2);
    }
    template<std::size_t... Is> // _M_Size > begin > number
    inline void _shift_backward(const size_type begin, const size_type number, const std::index_sequence<Is...>){
        ((std::move(std::get<Is>(_M_Data).get()+begin, std::get<Is>(_M_Data).get()+_M_Size, std::get<Is>(_M_Data).get()+begin-number)), ...);
    }
    template<std::size_t... Is> // _M_Size > begin > number?
    inline void _shift_forward(const size_type begin, const size_type number, const std::index_sequence<Is...>){
        ((std::move_backward(std::get<Is>(_M_Data).get()+begin, std::get<Is>(_M_Data).get()+_M_Size, std::get<Is>(_M_Data).get()+begin+number)), ...);
    }

    template<std::size_t... Is>
    inline void _move(const size_type i, const Parallel& ...args, const std::index_sequence<Is...>){
        ((std::get<Is>(_M_Data)[i] = std::move(std::get<Is>(std::forward_as_tuple(args...)))), ...);
    }
    template<std::size_t... Is>
    inline void _move(const size_type i, const std::tuple<Parallel...>& args, const std::index_sequence<Is...>){
        ((std::get<Is>(_M_Data)[i] = std::move(std::get<Is>(args))), ...);
    }

    template<std::size_t... Is>
    inline void _move(std::tuple<std::unique_ptr<Parallel[]>...>& new_data, const std::index_sequence<Is...>){
        ((std::move(std::get<Is>(_M_Data).get(), std::get<Is>(_M_Data).get()+_M_Size, std::get<Is>(new_data).get())), ...);
    }
    inline void _alloc(const size_type new_capacity){
        std::tuple<std::unique_ptr<Parallel[]>...> new_data{
            std::make_unique<Parallel[]>(new_capacity)...
        };
        _move(new_data, sequence);
        _M_Capacity = new_capacity;
        _M_Data = std::move(new_data);
    }
    template<typename Type>
    inline static std::ostream& print_parallel_vector_span(std::ostream& os, const Type* const e, const uint& sz){
        os << '[';
        if(sz) for(size_t i = 0; ;){
            os << e[i];
            if(++i == sz) break;
            os << ", ";
        }
        return os << ']';
    }
    template<std::size_t ...Is>
    inline void print_parallel_vector(std::ostream& os, const std::index_sequence<Is...>) const{
        os << "(" << size() << "/"<<capacity()<<")[\n";
        ((print_parallel_vector_span(os << "    ", std::get<Is>(_M_Data).get(), _M_Size) << '\n'), ...);
        os << "]";
    }

    friend inline std::ostream& operator<<(std::ostream& os, const parallel_vector<Parallel...>& e){
        e.print_parallel_vector(os, e.sequence);
        return os;
    }
    size_type _M_Size       = 0;
    size_type _M_Capacity   = 0;
    std::tuple<std::unique_ptr<Parallel[]>...> _M_Data;
};


