#pragma once

#include <algorithm>
#include <cstddef>
#include <memory>
#include <ostream>
#include <span>
#include <sstream>
#include <stdexcept>
#include <tuple>
#include <utility>
#include <limits>


template<typename ...Ts>
struct multi_swap_vector{
public:
    using size_type = unsigned int;
    using index_type = size_type;

    using ip_array = std::unique_ptr<std::shared_ptr<index_type>[]>;

    using p_tuple = std::tuple<Ts*...>;
    using cp_tuple = std::tuple<const Ts*...>;
    using up_tuple = std::tuple<std::unique_ptr<Ts[]>...>;
    using span_tuple = std::tuple<std::span<Ts>...>;
    using cspan_tuple = std::tuple<std::span<Ts>...>;
    using val_tuple = std::tuple<Ts...>;
    using ref_tuple = std::tuple<Ts&...>;
    using cref_tuple = std::tuple<const Ts&...>;
    inline static const auto sequence = std::index_sequence_for<Ts...>{};

    struct handle : public std::weak_ptr<index_type>{
        operator bool()const{return !expired();}

        friend std::ostream& operator<<(std::ostream& os, const handle& e){
            return os << e.lock();
        }
    };

    multi_swap_vector() = default;
    ~multi_swap_vector() = default;
    multi_swap_vector(const multi_swap_vector&) = delete;
    multi_swap_vector& operator=(const multi_swap_vector&) = delete;
    inline multi_swap_vector(multi_swap_vector&& mv):_M_Size(mv._M_Size), _M_Capacity(mv._M_Capacity), _M_Data(std::move(mv._M_Data)), _M_Index(std::move(mv._M_Index)) {
        mv._M_Size      = 0;
        mv._M_Capacity  = 0;
    }
    inline multi_swap_vector& operator=(multi_swap_vector&& mv){
        _M_Size = mv._M_Size;
        _M_Capacity = mv._M_Capacity;
        _M_Data = std::move(mv._M_Data);  
        _M_Index = std::move(mv._M_Index); 
        mv._M_Size      = 0;
        mv._M_Capacity  = 0;
        return *this;
    }
    handle append(const val_tuple& v){
        _expand();
        _set(_M_Size, v, sequence);
        handle h{(_M_Index[_M_Size] = std::make_shared<index_type>(_M_Size))};
        _M_Size++;
        return h;
    }
    void set(const handle& h, const val_tuple& v){
        if(auto p = h.lock()) 
            _set(*p, v, sequence);
    }
    bool set_cmp(const handle& h, const val_tuple& v){
        if(auto p = h.lock()) 
            return _set_cmp(*p, v, sequence);
        return false;
    }


    inline p_tuple      data(){return _data(sequence);}
    inline cp_tuple     data()const{return _data(sequence);}
    template<typename T>        inline T* data(){return std::get<T>(_M_Data).get();}
    template<typename T>        inline const T* data()const{return std::get<T>(_M_Data).get();}
    template<std::size_t Idx>   inline auto* data(){return std::get<Idx>(_M_Data).get();}
    template<std::size_t Idx>   inline const auto* data()const{return std::get<Idx>(_M_Data).get();}

    inline span_tuple   span(){return _span(sequence);}
    inline cspan_tuple  span()const{return _span(sequence);}

    inline span_tuple   span_size(){return _span(sequence);}
    inline cspan_tuple  span_size()const{return _span(sequence);}

    inline span_tuple   span_capacity(){return _capacity_span(sequence);}
    inline cspan_tuple  span_capacity()const{return _capacity_span(sequence);}

    inline ref_tuple   operator[](const handle& h){return _get(h);}
    inline cref_tuple  operator[](const handle& h)const{return _get(h);}

    inline ref_tuple get(const handle& h){return _get(h, sequence);}
    inline cref_tuple get(const handle& h)const{return _get(h, sequence);}

    template<typename T>        inline T* get(){return std::get<T>(_M_Data).get();}
    template<typename T>        inline const T* get()const{return std::get<T>(_M_Data).get();}
    template<std::size_t Idx>   inline auto* get(){return std::get<Idx>(_M_Data).get();}
    template<std::size_t Idx>   inline const auto* get()const{return std::get<Idx>(_M_Data).get();}

    template<typename T>        inline T& get(const index_type i){return std::get<T>(_M_Data)[i];}
    template<typename T>        inline const T& get(const index_type i)const{return std::get<T>(_M_Data)[i];}
    template<std::size_t Idx>   inline auto& get(const index_type i){return std::get<Idx>(_M_Data)[i];}
    template<std::size_t Idx>   inline const auto& get(const index_type i)const{return std::get<Idx>(_M_Data)[i];}

    inline static constexpr size_type max_size(){return std::numeric_limits<size_type>::max();}
    inline size_type size()const{return _M_Size;}
    inline size_type capacity()const{return _M_Capacity;}
    bool empty()const{return _M_Size==0;}

    inline std::ostream& print(std::ostream& os) const{
        os << '(' << size() << '/' << capacity() << ")[\n";
        _print_spans(os, sequence);
        return os << ']';
    }
    friend std::ostream& operator<<(std::ostream& os, const multi_swap_vector& e){
        return e.print(os);
    }
private:

    template<typename T> inline static void _print_span(std::ostream& os, const T* s, const size_type size){
        os << "    [";
        if(size) for(size_t i = 0; ;){
            os << s[i];
            if(++i == size) break;
            os << ", ";
        }
        os << "]\n";
    }
    template<std::size_t ...Is> inline void _print_spans(std::ostream& os, std::index_sequence<Is...>) const{
        const size_type size_ = size();
        (_print_span(os, get<Is>(), size_), ...);
    }


    template<std::size_t ...Is> inline p_tuple _data(std::index_sequence<Is...>){return p_tuple{std::get<Is>(_M_Data).get()...};}
    template<std::size_t ...Is> inline cp_tuple _data(std::index_sequence<Is...>)const{return cp_tuple{std::get<Is>(_M_Data).get()...};}

    template<std::size_t ...Is> inline span_tuple _span(std::index_sequence<Is...>){return span_tuple{{std::get<Is>(_M_Data).get(), _M_Size}...};}
    template<std::size_t ...Is> inline cspan_tuple _span(std::index_sequence<Is...>)const{return cspan_tuple{{std::get<Is>(_M_Data).get(), _M_Size}...};}

    template<std::size_t ...Is> inline span_tuple _capacity_span(std::index_sequence<Is...>){return span_tuple{{std::get<Is>(_M_Data).get(), _M_Size}...};}
    template<std::size_t ...Is> inline cspan_tuple _capacity_span(std::index_sequence<Is...>)const{return cspan_tuple{{std::get<Is>(_M_Data).get(), _M_Size}...};}

    template<std::size_t ...Is> inline void _set(const index_type i, const val_tuple& ref, std::index_sequence<Is...>){((std::get<Is>(_M_Data)[i] = std::get<Is>(ref)), ...);}


    template<std::size_t ...Is> inline bool _set_cmp(const index_type i, const val_tuple& ref, std::index_sequence<Is...>){
        bool changed = false;
        ((
            std::get<Is>(_M_Data)[i] != std::get<Is>(ref) ? 
                void((changed = true, std::get<Is>(_M_Data)[i] = std::get<Is>(ref)))
                : void()
        ),...);
        return changed;
    }

    template<std::size_t ...Is>
    inline ref_tuple _get(const handle& h, std::index_sequence<Is...>){
        auto index_pointer = h.lock();
        if(!index_pointer) 
            throw std::runtime_error((std::ostringstream() << "multi_swap_vector<" << typeid(multi_swap_vector).name() << ">::get: 'handle has expired'\n").str());
        index_type index = *index_pointer;
        return ref_tuple{
            std::get<Is>(_M_Data)[index]...
        };
    }
    template<std::size_t ...Is>
    inline cref_tuple _get(const handle& h, std::index_sequence<Is...>) const{
        auto index_pointer = h.lock();
        if(!index_pointer) 
            throw std::runtime_error((std::ostringstream() << "multi_swap_vector<" << typeid(multi_swap_vector).name() << ">::get: 'handle has expired'\n").str());
        index_type index = *index_pointer;
        return ref_tuple{
            std::get<Is>(_M_Data)[index]...
        };
    }
    template<std::size_t ...Is>
    inline static void _move_tuple(const up_tuple& from, up_tuple& to, const size_type n, std::index_sequence<Is...>){
        ((std::move(std::get<Is>(from).get(), std::get<Is>(from).get()+n, std::get<Is>(to).get()), ...));
    }
    inline void _expand(){
        if(_M_Size >= _M_Capacity)
            _alloc(_M_Capacity <= 8 ? 16 : _M_Capacity*2);
    }
    inline void _expand(const size_type num){
        size_type min_capacity = _M_Size+num;
        if(min_capacity  > _M_Capacity)
            _alloc(std::max(_M_Capacity <= 8 ? 16 : _M_Capacity*2, min_capacity));
    }
    inline void _shrink(){
        if(_M_Size < _M_Capacity>>1 && _M_Capacity > 32) 
            _alloc(_M_Capacity>>1);
    }
    inline void _alloc(const size_type new_capacity){
        up_tuple new_data{std::make_unique<Ts[]>(new_capacity)...};
        _move_tuple(_M_Data, new_data, _M_Size, sequence);
        _M_Data = std::move(new_data);

        ip_array new_indexes = std::make_unique<std::shared_ptr<index_type>[]>(new_capacity);
        std::move(_M_Index.get(), _M_Index.get() + _M_Size, new_indexes.get());
        _M_Index = std::move(new_indexes);

        _M_Capacity = new_capacity;
    }
    size_type     _M_Size = 0;
    size_type     _M_Capacity = 0;
    up_tuple      _M_Data;
    ip_array      _M_Index;
};
