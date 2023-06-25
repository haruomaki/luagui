#include <GUI.hpp>

#include <base.hpp>

struct MyC {
    friend ostream &operator<<(ostream &os, const MyC &input) {
        os << "MyCだおw";
        return os;
    }
};

int main() {
    debug(234);

    MyC myc;
    debug(myc);

    auto v = glm::vec3{1, 2, 3};
    debug(v);
}
