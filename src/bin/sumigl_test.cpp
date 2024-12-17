#include <sumigl.hpp>

int main() {
    GL::Context ctx;
    debug(ctx.dpi());
    ctx.create_window(600, 600, "aaaa");
    ctx.mainloop();
}
