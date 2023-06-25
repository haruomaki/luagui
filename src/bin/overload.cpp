#include <base.hpp>

template <typename... Args>
class OverloadClass {
    int value_ = 0;

  public:
    template <typename T>
    void overload(T arg) {
        overloadImpl(arg);
    }

    auto operator+(const auto &other) const {
        return value_ + other;
    }

    OverloadClass &operator=(const auto &other) {
        value_ = other;
        return *this;
    }

    OverloadClass &operator+=(const auto &other) {
        return *this = *this + other;
    }

    friend ostream &operator<<(ostream &os, const OverloadClass &self) {
        os << "<OverloadClass: " << self.value_ << ">";
        return os;
    }

  private:
    template <typename T>
    void overloadImpl(T arg) {
        std::cout << "Overload with " << typeid(T).name() << ": " << arg << std::endl;
    }

    // template <typename T, typename... Rest>
    // void overloadImpl(T arg, Rest... rest) {
    //     overloadImpl(arg);
    //     overloadImpl(rest...);
    // }
};

int main() {
    OverloadClass<int, float, char, char, char> obj;
    obj.overload(42);
    obj.overload(3.14f);
    obj.overload('A');
    obj.overload("fa");
    // obj.overload(42, 3.14f, 'A');

    vector<int> vec = {3, 1, 4};

    debug(obj += 9);
    debug(obj + "a");
    debug(obj + "ai");
    // debug(obj += vec);

    return 0;
}
