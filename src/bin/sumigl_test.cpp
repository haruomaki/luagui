#include <sumigl.hpp>

int main() {
    GL::Sumi sumi;
    debug(sumi.dpi());
    sumi.create_window(600, 600, "aaaa");
    sumi.mainloop();
}
