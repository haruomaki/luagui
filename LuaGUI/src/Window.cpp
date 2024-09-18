#include "Window.hpp"

using Keymap = std::unordered_map<std::string_view, int>;

static const Keymap &keymap() {
    // コンストラクタ・デストラクタの例外を捕捉できるよう、グローバル変数でなくローカルstatic変数にする
    // 明示的なnewでstatic変数のデストラクタを回避する（未定義のデストラクタ順を避けるため）
    static const auto *keymap_inner = new Keymap{
        {"Space", GLFW_KEY_SPACE},
        {"A", GLFW_KEY_A},
        {"B", GLFW_KEY_B},
        {"C", GLFW_KEY_C},
        {"K", GLFW_KEY_K},
        {"Escape", GLFW_KEY_ESCAPE},
        {"Enter", GLFW_KEY_ENTER},
        {"Right", GLFW_KEY_RIGHT},
        {"Left", GLFW_KEY_LEFT},
        {"Down", GLFW_KEY_DOWN},
        {"Up", GLFW_KEY_UP},
        // TODO: 他のキーも随時追加
    };
    return *keymap_inner;
}

void register_window(sol::state &lua) {
    lua.set_function("GetKey", [&lua](const char *key) -> bool {
        Window &window = lua["__CurrentWindow"];

        std::string_view key_view(key);
        auto it = keymap().find(key_view);
        if (it == keymap().end()) {
            warn("キー名が不正です: ", key);
            return false;
        }
        int keycode = it->second;
        return window.key(keycode);
    });
}
