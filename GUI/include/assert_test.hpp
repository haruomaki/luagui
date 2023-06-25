#include <functional>
#include <iostream>

template <class T>
void f1(T x) {
    std::cout << x * 2 << std::endl;
}

template <class T>
void f2(T x) {
    f1(x);
}

class Property {
    int value_ = 0;

  public:
    void setValue(int value) {
        value_ = value;
    }

    auto operator+(const auto &x) const {
        return value_ + x;
    }

    Property &operator=(const auto &x) {
        setValue(x + 1);
        return *this;
    }

    Property &operator+=(const auto &x) {
        return *this = *this + x;
    }
};
