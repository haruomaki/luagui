#include <iostream>

template <typename T>
void show(const T &x) {
    std::cout << x << std::endl;
}

int main() {
    int x;
    show(x);
}

// template <typename T>
// void show(T &&x) {
//     std::cout << std::forward<T>(x) << std::endl;
// }

// inline void show(const int *x) {
//     std::cout << *x << std::endl;
// }
