#include <base.hpp>

template <auto x>
class Base {
    using PrivateType = float;

  protected:
    using MyType = decltype(x);
};

template <auto x>
class Derived : Base<334> {
    MyType y;
};

int main() {
}
