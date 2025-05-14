#pragma once



#include <algorithm>
#include <cstddef>
#include <glm/ext/vector_float2.hpp>
#include <iostream>
#include <memory>
#include <ostream>
#include <span>
#include <sys/types.h>
#include <tuple>
#include <type_traits>
#include <utility>


template<typename ...Parallel>
struct parallel_vector{
public:
    using size_type = uint;
    inline static const auto sequence = std::index_sequence_for<Parallel...>{};



    void push_back(const std::tuple<Parallel...>& value = {}){
        _expand();
        _move(_M_Size++, value, sequence);
        return;
    }

    void pop_back(){
        _M_Size--;
        _shrink();
        return;
    }
    void remove(size_type index){
        if(index == _M_Size-1) return pop_back();
        _shift_backward(index+1, 1, sequence);
        _M_Size--;
        _shrink();
        return;
    }
    void emplace(size_type index, const std::tuple<Parallel...>& value){
        _expand();
        _shift_forward(index, 1, sequence);
        _move(index,value, sequence);
        _M_Size++;
        return;
    }
    const size_type& size(){
        return _M_Size;
    }
    const size_type& capasity(){
        return _M_Capasity;
    }
    const size_type& size()const{
        return _M_Size;
    }
    const size_type& capasity()const{
        return _M_Capasity;
    }
    const std::tuple<std::unique_ptr<Parallel[]>...>& data(){
        return _M_Data;
    }
private:
    void _expand(){
        if(_M_Size >= _M_Capasity)
            _alloc(_M_Capasity <= 8 ? 16 : _M_Capasity*2);
    }
    void _shrink(){
        if(_M_Size/2 < _M_Capasity && _M_Capasity > 32) 
            _alloc(_M_Capasity/2);
    }
    template<size_type Is>
    inline void _set(size_type i, const auto& args){
        std::get<Is>(_M_Data)[i] = args;
    }
    inline void _set(size_type i, const auto& args){
        std::get<std::remove_all_extents_t<decltype(args)>>(_M_Data)[i] = args;
    }
    template<std::size_t... Is> // _M_Size > begin > number
    inline void _shift_backward(size_type begin, size_type number, std::index_sequence<Is...>){
        ((std::move(std::get<Is>(_M_Data).get()+begin, std::get<Is>(_M_Data).get()+_M_Size, std::get<Is>(_M_Data).get()+begin-number)), ...);
    }
    template<std::size_t... Is> // _M_Size > begin > number?
    inline void _shift_forward(size_type begin, size_type number, std::index_sequence<Is...>){
        ((std::move_backward(std::get<Is>(_M_Data).get()+begin, std::get<Is>(_M_Data).get()+_M_Size, std::get<Is>(_M_Data).get()+begin+number)), ...);
    }
    template<std::size_t... Is>
    inline void _move(size_type i, const std::tuple<Parallel...>& args, std::index_sequence<Is...>){
        ((std::get<Is>(_M_Data)[i] = std::move(std::get<Is>(args))), ...);
    }
    template<std::size_t... Is>
    inline void _move(std::tuple<std::unique_ptr<Parallel[]>...>& new_data, std::index_sequence<Is...>){
        ((std::move(std::get<Is>(_M_Data).get(), std::get<Is>(_M_Data).get()+_M_Size, std::get<Is>(new_data).get())), ...);
    }
    inline void _alloc(size_type new_capasity){
        std::tuple<std::unique_ptr<Parallel[]>...> new_data{
            std::make_unique<Parallel[]>(new_capasity)...
        };
        _move(new_data, sequence);
        _M_Capasity = new_capasity;
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
    inline void print_parallel_vector(std::ostream& os, std::index_sequence<Is...>) const{
        os << "(" << size() << "/"<<capasity()<<")[\n";
        ((print_parallel_vector_span(os << "    ", std::get<Is>(_M_Data).get(), _M_Size) << '\n'), ...);
        os << "]";
    }

    friend inline std::ostream& operator<<(std::ostream& os, const parallel_vector<Parallel...>& e){
        e.print_parallel_vector(os, e.sequence);
        return os;
    }
    size_type _M_Size       = 0;
    size_type _M_Capasity   = 0;
    std::tuple<std::unique_ptr<Parallel[]>...> _M_Data;
};


