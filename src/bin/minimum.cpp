#include <iostream>

int main() {
    setUTF8();

    std::cout << "nihongo? ss み aにbまcむd\n"
              << std::endl;

    std::cout << "abcdef 日本語も表示（ひょうじ）できます。\n";

    GL::Context ctx;
    Window window(ctx, 500, 400, "たいとる");
    ctx.mainloop();
}
