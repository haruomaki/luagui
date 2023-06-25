#include <functional>
#include <iostream>

class Property {
  protected:
    int value_ = 0;

  public:
    void show() const {
        std::cout << value_ << std::endl;
    }

    void setValue(int value) {
        value_ = value;
    }

    auto operator+(const auto &x) const {
        return value_ + x;
    }

    Property &operator=(const auto &x) {
        setValue(x);
        return *this;
    }

    Property &operator+=(const auto &x) {
        return *this = *this + x;
    }
};

class Derived : public Property {
  public:
    void showPrettily() {
        std::cout << "値は" << value_ << "だよ" << std::endl;
    }
};

template <typename T>
    requires std::is_arithmetic_v<T>
void f() {
    std::cout << "処理は型に関係ないけどね" << std::endl;
}
