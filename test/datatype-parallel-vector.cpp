#include "../lib/datatypes/parallel-vector.hpp"
#include <iostream>


struct a{
    int a, b, c;

    friend std::ostream& operator<<(std::ostream& os, const struct a& e){
        return os << "[" << e.a << ", " << e.b << ", " << e.c << "]";
    }
};
struct b{
    void* a = nullptr;

    friend std::ostream& operator<<(std::ostream& os, const b& e){
        return os << e.a;
    }
};
int main(int argc, const char* argv[]){
    parallel_vector<int, float, a, b> pv;

    std::cout << pv << '\n';
    pv.push_back({1, 2.f, {1,2,3}, {}});
    std::cout << pv << '\n';
    pv.push_back({6, 6.66f, {6,6,6}, {reinterpret_cast<void*>(0x666666UL)}});
    pv.push_back(7, 7.66f, {7,7,7}, {reinterpret_cast<void*>(0x7UL)});
    std::cout << pv << '\n';
    std::cout << "pv[0][1]" << pv.get<0>(1) << '\n';
    std::cout << "pv[1][1]" << pv.get<1>(1) << '\n';
    std::cout << "pv[2][1]" << pv.get<2>(1) << '\n';
    std::cout << "pv[3][1]" << pv.get<3>(1) << '\n';

    pv.remove(1);
    std::cout << pv << '\n';

    return 0;
}