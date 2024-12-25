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
    GUI gui;
    auto window = gui.resources.append<Window>(gui, 500, 400, "minimum.cppのウィンドウ");
    World &world = gui.create_world(); // TODO: いちいちワールドを作らなくてもいいようにしたい
    world.add_component<UpdateComponent>([&](auto & /*self*/) {
        if (window.get().key_down()[GLFW_KEY_W]) std::cout << "わん🐶\n";
        if (window.get().key_down()[GLFW_KEY_N]) std::cout << "にゃん🐱\n";
        if (window.get().key(GLFW_KEY_Q)) window.get().close();
    });
    gui.mainloop();
}
