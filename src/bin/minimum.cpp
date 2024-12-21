#include <gui.hpp>

int main() {
    setUTF8();

    std::cout << "The quick brown fox jumps over the lazy dog.\n";
    std::cout << "素早い茶色の狐が怠け者の犬を飛び越える。\n";
    std::cout << "1234567890!@#$%^&*()_+-=[]{}|;:'\",.<>?/\\\n";

    std::cout << "☕⚡⏏️";                                           // Unicode 4.0
    std::cout << "🤯🧔🏽🥪";                                        // Unicode 10.0
    std::cout << "🥲🦣🫑";                                          // Unicode 13.0
    std::cout << "🫠🧌🪪";                                          // Unicode 14.0
    std::cout << "🫨🩵🪼";                                          // Unicode 15.0
    std::cout << "👁️‍🗨️🏳️‍⚧️🏳️‍🌈"; // ZWJ
    std::cout << "\n";

    // ただウィンドウを作成し、Qキーで終了するプログラム。
    GL::Context ctx;
    Window window(ctx, 500, 400, "minimum.cppのウィンドウ");
    World &world = window.create_world();      // TODO: いちいちワールドを作らなくてもいいようにしたい
    auto &cam = world.add_component<Camera>(); // TODO: いちいちカメラを作りたくない
    world.rendering_camera() = &cam;
    world.add_component<UpdateComponent>([&](auto & /*self*/) {
        if (window.key_down()[GLFW_KEY_W]) std::cout << "わん🐶\n";
        if (window.key_down()[GLFW_KEY_N]) std::cout << "にゃん🐱\n";
        if (window.key(GLFW_KEY_Q)) window.close();
    });
    ctx.mainloop();
}
