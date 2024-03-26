#include <iostream>

struct Extra {
    int b = 2;
};

template <bool inherit>
struct MyClass {
    int a = 1;
    void print() {
        std::cout << a << "\n";
    }
};

template <>
struct MyClass<true> : Extra {
    void print() {
        std::cout << a << " and " << b << "\n";
    }
};

int main() {
    MyClass<false> my1;
    my1.print(); // 1

    MyClass<true> my2;
    my2.print(); // 1 and 2
}
