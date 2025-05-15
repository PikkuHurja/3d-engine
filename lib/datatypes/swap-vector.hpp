#pragma once





#include <algorithm>
#include <alloca.h>
#include <initializer_list>
#include <limits>
#include <memory>
#include <span>
#include <stdexcept>
#include <utility>

//no copy, since handles would be not created
//no initializer list, since handles would be not created
/*
    Guranteed to have all enums concurrent
    Accessor is a pointer to an integer which specifies the index to use etc, etc etc
    O(1) remove and insertion in the common case
    Order is not preserved, handle is important and cannot be recreated if lost, but can be duplicated
*/
template<typename T>
struct swap_vector{
public:
    using size_type = unsigned int;
    using index_type = size_type;
    
    struct handle : public std::weak_ptr<index_type>{
        operator bool()const{return !expired();}
    };
    swap_vector() = default;
    ~swap_vector() = default;


    inline swap_vector(const swap_vector& v) = delete;
    inline swap_vector& operator=(const swap_vector& v) = delete;

    inline swap_vector(swap_vector&& mv):_M_Size(mv._M_Size), _M_Capacity(mv._M_Capacity), _M_Data(std::move(mv._M_Data)), _M_Index(std::move(mv._M_Index)) {
        mv._M_Size = 0;
        mv._M_Capacity = 0;
    }
    inline swap_vector& operator=(swap_vector&& mv){
        _M_Size = mv._M_Size;
        _M_Capacity = mv._M_Capacity;
        _M_Data = std::move(mv._M_Data);
        _M_Index = std::move(mv._M_Index);

        mv._M_Size = 0;
        mv._M_Capacity = 0;
        return *this;
    }
    swap_vector(size_type reserve):_M_Size(0),_M_Capacity(reserve),_M_Data(std::make_unique<T[]>(reserve)), _M_Index(std::make_unique<std::shared_ptr<index_type>[]>(reserve)){}



    T& operator[](const handle& h){return get(h);}
    const T& operator[](const handle& h)const{return get(h);}
    T& get(const handle& h){
        auto p = h.lock();
        if(!p) throw std::runtime_error("Expired handle for swap_vector<...>");
        return _M_Data[*p];
    }
    const T& get(const handle& h) const{
        auto p = h.lock();
        if(!p) throw std::runtime_error("Expired handle for swap_vector<...>");
        return _M_Data[*p];
    }
    void set(const handle& h, const T& v){
        auto p = h.lock();
        if(!p) throw std::runtime_error("Expired handle for swap_vector<...>");
        _M_Data[*p] = v;
    }
    void set(const handle& h, T&& v){
        auto p = h.lock();
        if(!p) throw std::runtime_error("Expired handle for swap_vector<...>");
        _M_Data[*p] = std::move(v);
    }

    handle append(const T& value){
        _expand();
        _M_Data[_M_Size] = value;
        handle h{(_M_Index[_M_Size] = std::make_shared<index_type>(_M_Size))};
        _M_Size++;
        return h;
    }

    handle append(T&& value){
        _expand();
        _M_Data[_M_Size] = std::move(value);
        handle h{(_M_Index[_M_Size] = std::make_shared<index_type>(_M_Size))};
        _M_Size++;
        return h;
    }

    void append(const T& value, size_type num, handle* handles){
        _expand(num);
        for(size_type i = 0; i < num; i++){
            _M_Data[_M_Size+i] = value;
            handles[i] = handle{_M_Index[_M_Size+i] = std::make_shared<index_type>(_M_Size+i)};
        }
        _M_Size+=num;
    }

    void remove(const handle& i){
        auto shpt = i.lock();
        if(!shpt) return;
        auto index = *shpt;
        
        --_M_Size;
        if(index==_M_Size){    //no need to swap
            std::destroy_at(_M_Data[index]);       //clear
            _M_Index[index].reset();    //clear
        }else{
                //swap
            _M_Data[index] = _M_Data[_M_Size];  //override data,
            *(_M_Index[index] = _M_Index[_M_Size]) = index; //override index and reset the index in the handles ptr
        }
        _shrink();
    }

    void remove(const handle* handles, size_type num){
        for(size_t i = 0; i < num; i++)if(auto shpt = handles[i].lock()){
            auto index = *shpt;
            --_M_Size;
            if(index==_M_Size){    //no need to swap
                std::destroy_at(_M_Data[index]);       //clear
                _M_Index[index].reset();    //clear
            }else{
                    //swap
                _M_Data[index] = _M_Data[_M_Size];  //override data,
                *(_M_Index[index] = _M_Index[_M_Size]) = index; //override index and reset the index in the handles ptr
            }
        }
        _shrink();
    }

    void clear(){
        _M_Size = 0;
        _M_Capacity = 0;
        _M_Data.reset();
        _M_Index.reset();
    }
        //resizes capacity exactly to the said size, does not create or initialize datamembers bcs no handles
    void resize(size_type sz){
        if(_M_Capacity == sz) return;
        if(_M_Size > sz) _M_Size = sz;
        _alloc(sz);
    }

    bool empty(){return _M_Size==0;}

    T* data(){return _M_Data.get();}
    const T* data()const {return _M_Data.get();}

    std::span<T> span(){return {_M_Data.get(), _M_Size};}
    std::span<const T> span()const {return {_M_Data.get(), _M_Size};}

        //span, thats the whole capacity size
    std::span<T> span_capacity(){return {_M_Data.get(), _M_Capacity};}
    std::span<const T> span_capacity()const {return {_M_Data.get(), _M_Capacity};}

    inline void shrink_to_fit(){
        if(_M_Size == _M_Capacity) return;
        _alloc(_M_Size);
    }
    inline static constexpr size_type max_size(){return std::numeric_limits<size_type>::max();}
    inline size_type size()const{return _M_Size;}
    inline size_type capacity()const{return _M_Capacity;}

    T& front(){return *_M_Data;}
    T& back(){return _M_Data[_M_Size-1];}


private:

    void _expand(){
        if(_M_Size >= _M_Capacity)
            _alloc(_M_Capacity <= 8 ? 16 : _M_Capacity*2);
    }
    void _expand(size_type num){
        size_type min_capacity = _M_Size+num;
        if(min_capacity  > _M_Capacity)
            _alloc(std::max(_M_Capacity <= 8 ? 16 : _M_Capacity*2, min_capacity));
    }
    void _shrink(){
        if(_M_Size < _M_Capacity>>1 && _M_Capacity > 32) 
            _alloc(_M_Capacity>>1);
    }
    void _alloc(size_type new_capacity){
        std::unique_ptr<T[]> new_data = std::make_unique<T[]>(new_capacity);
        std::unique_ptr<T[]> new_indexes = std::make_unique<std::shared_ptr<index_type>[]>(new_capacity);
        std::move(_M_Data.get(), _M_Data.get() + _M_Size, new_data.get());
        std::move(_M_Index.get(), _M_Index.get() + _M_Size, new_indexes.get());

        _M_Data = std::move(new_data);
        _M_Index = std::move(new_indexes);
        _M_Capacity = new_capacity;
    }

    size_type                                           _M_Size = 0;
    size_type                                           _M_Capacity = 0;
    std::unique_ptr<T[]>                                _M_Data;
    std::unique_ptr<std::shared_ptr<index_type>[]>      _M_Index;
};