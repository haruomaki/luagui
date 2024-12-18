#include <iostream>

int main() {
    setUTF8();

    std::cout << "The quick brown fox jumps over the lazy dog.\n";
    std::cout << "素早い茶色の狐が怠け者の犬を飛び越える。\n";
    std::cout << "1234567890!@#$%^&*()_+-=[]{}|;:'\",.<>?/\\\n";

    std::cout << "🥲🦣🫑"; // Unicode 13.0
    std::cout << "🫠🧌🪪"; // Unicode 14.0
    std::cout << "🫨🩵🪼"; // Unicode 15.0
    std::cout << "\n";

    std::cout << "���👁️‍🗨️🏳️‍⚧️🏳️‍🌈"; // ZWJ

    // ただウィンドウを作成し、Qキーで終了するプログラム。
    // FIXME: 終了時、malloc_consolidate(): unaligned fastbin chunk detectedというエラーが出るので何とかする
    GL::Context ctx;
    Window window(ctx, 500, 400, "minimum.cppのウィンドウ");
    World &world = window.create_world();            // TODO: いちいちワールドを作らなくてもいいようにしたい
    world.append_child<NormalCamera>().set_active(); // TODO: いちいちカメラを作りたくない
    world.add_component<UpdateComponent>([&](auto & /*self*/) {
        if (window.key(GLFW_KEY_Q)) window.close();
    });
    ctx.mainloop();
}
