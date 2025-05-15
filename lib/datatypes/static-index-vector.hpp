#pragma once




//template<typename T>

#include <memory>
#include <sys/types.h>
struct static_index_vector{
public:
    using T = uint;
    using size_type = uint;
    using index_type = size_type;
private:


    size_type   _M_Size     = 0;
    size_type   _M_Capasity = 0;

    std::unique_ptr<T[]>            _M_Data;
    std::unique_ptr<size_type[]>    _M_Indexes;
};