#include <base.hpp>

class Base {
  public:
    int x_ = 0;
};

class DerivedA : virtual Base {
};

class DerivedX : public virtual Base, DerivedA {
};

int main() {
    DerivedX x;

    debug(x.x_);
}
