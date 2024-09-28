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
        {"D", GLFW_KEY_D},
        {"E", GLFW_KEY_E},
        {"F", GLFW_KEY_F},
        {"G", GLFW_KEY_G},
        {"H", GLFW_KEY_H},
        {"I", GLFW_KEY_I},
        {"J", GLFW_KEY_J},
        {"K", GLFW_KEY_K},
        {"L", GLFW_KEY_L},
        {"M", GLFW_KEY_M},
        {"N", GLFW_KEY_N},
        {"O", GLFW_KEY_O},
        {"P", GLFW_KEY_P},
        {"Q", GLFW_KEY_Q},
        {"R", GLFW_KEY_R},
        {"S", GLFW_KEY_S},
        {"T", GLFW_KEY_T},
        {"U", GLFW_KEY_U},
        {"V", GLFW_KEY_V},
        {"W", GLFW_KEY_W},
        {"X", GLFW_KEY_X},
        {"Y", GLFW_KEY_Y},
        {"Z", GLFW_KEY_Z},
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

static inline std::optional<int> from_name(const char *name) {
    std::string_view name_view(name);
    auto it = keymap().find(name_view);
    if (it == keymap().end()) {
        warn("キー名が不正です: ", name);
        return std::nullopt;
    }
    return it->second;
}

class Screen {};

void register_window(sol::state &lua) {
    lua.set_function("GetKey", [&lua](const char *key) -> bool {
        Window &window = lua["__CurrentWindow"];
        auto keycode = from_name(key);
        return keycode.has_value() ? window.key(*keycode) : false;
    });

    lua.set_function("GetKeyDown", [&lua](const char *key) -> bool {
        Window &window = lua["__CurrentWindow"];
        auto keycode = from_name(key);
        return keycode.has_value() ? window.key_down()[*keycode] : false;
    });

    lua.set_function("GetKeyUp", [&lua](const char *key) -> bool {
        Window &window = lua["__CurrentWindow"];
        auto keycode = from_name(key);
        return keycode.has_value() ? window.key_up()[*keycode] : false;
    });

    // ウィンドウを閉じる関数
    lua.set_function("CloseWindow", [&lua] {
        Window &window = lua["__CurrentWindow"];
        window.close();
    });

    // リフレッシュレートを取得する関数
    lua.new_usertype<Screen>(
        "Screen",
        "refreshRate", sol::readonly_property([&lua]() {
            Window &window = lua["__CurrentWindow"];
            return window.refresh_rate();
        }));
}
