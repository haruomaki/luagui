// #include <iostream>

// template <typename... Args>
// class OverloadClass {
//     template <typename T>
//     void overloadImpl(T arg) {
//         std::cout << "Overload with " << typeid(T).name() << ": " << arg << std::endl;
//     }

//   public:
//     void overload(Args... args) {
//         overloadImpl(args...);
//     }
// };

// int main() {
//     OverloadClass<int, float, char> obj;
//     obj.overload(42,4.F,'A');
//     obj.overload(3.14f);
//     obj.overload('A');

//     return 0;
// }

#include <iostream>

template <typename... Args>
class OverloadClass {
  public:
    template <typename T>
    void overload(T arg) {
        overloadImpl(arg);
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

    return 0;
}
