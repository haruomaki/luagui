#include <base.hpp>

class Base {
    int value_ = 0;

  public:
    auto operator+(const auto &other) const {
        return value_ + other;
    }

    Base &operator=(const auto &other) {
        value_ = other;
        return *this;
    }

    Base &operator+=(const auto &other) {
        return *this = *this + other;
    }

    friend ostream &operator<<(ostream &os, const Base &self) {
        os << "<OverloadClass: " << self.value_ << ">";
        return os;
    }
};

int main() {
    Base base;

    base = 7;
    // base = "aa";

    base + vector<int>{1, 2, 3};
}
