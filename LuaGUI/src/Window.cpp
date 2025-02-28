#include "Window.hpp"
#include <GUI/GUI.hpp>
#include <GUI/Window.hpp>
#include <sol/sol.hpp>

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
        {"LeftShift", GLFW_KEY_LEFT_SHIFT},
        {"LeftControl", GLFW_KEY_LEFT_CONTROL},
        {"LeftAlt", GLFW_KEY_LEFT_ALT},
        {"LeftSuper", GLFW_KEY_LEFT_SUPER},
        {"RightShift", GLFW_KEY_RIGHT_SHIFT},
        {"RightControl", GLFW_KEY_RIGHT_CONTROL},
        {"RightAlt", GLFW_KEY_RIGHT_ALT},
        {"RightSuper", GLFW_KEY_RIGHT_SUPER},
        {"Menu", GLFW_KEY_MENU},
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
class Mouse {};

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

    // Window.background_colorのような静的プロパティが欲しいので、__indexと__newindexを自作する。
    lua.new_usertype<Window>("Window");
    lua["Window"][sol::metatable_key]["__index"] = [&lua](const sol::table & /*tbl*/, sol::stack_object key) -> sol::object {
        // print("__indexだよ！", key.as<std::string>());
        Window &window = lua["__CurrentWindow"];
        if (key.as<std::string>() == "background_color") {
            return sol::make_object(lua, window.background_color);
        }
        return sol::nil;
    };
    lua["Window"][sol::metatable_key]["__newindex"] = [&lua](const sol::table & /*tbl*/, sol::stack_object key, sol::stack_object value) {
        // print("__newindexだよ！", key.as<std::string>());
        Window &window = lua["__CurrentWindow"];
        if (key.as<std::string>() == "background_color") {
            window.background_color = value.as<RGBA>();
        }
    };

    // リフレッシュレートを取得する関数
    lua.new_usertype<Screen>(
        "Screen",
        "refreshRate", sol::readonly_property([&lua]() {
            GUI &gui = lua["__GUI"];
            return gui.refresh_rate();
        }));

    // マウスカーソルに関する関数群
    lua.new_usertype<Mouse>(
        "Mouse",
        "pos", sol::readonly_property([&lua]() -> glm::vec2 {
            Window &window = lua["__CurrentWindow"];
            auto [x, y] = window.cursor_pos();
            return {x, y};
        }),
        "diff", sol::readonly_property([&lua]() -> glm::vec2 {
            Window &window = lua["__CurrentWindow"];
            auto [x, y] = window.cursor_diff();
            return {x, y};
        }),
        "left", sol::readonly_property([&lua]() -> bool {
            Window &window = lua["__CurrentWindow"];
            return window.mouse(GLFW_MOUSE_BUTTON_LEFT);
        }),
        "right", sol::readonly_property([&lua]() -> bool {
            Window &window = lua["__CurrentWindow"];
            return window.mouse(GLFW_MOUSE_BUTTON_RIGHT);
        }),
        "LeftDown", [&lua]() -> bool {
            Window &window = lua["__CurrentWindow"];
            return window.mouse_down()[GLFW_MOUSE_BUTTON_LEFT]; },
        "RightDown", [&lua]() -> bool {
            Window &window = lua["__CurrentWindow"];
            return window.mouse_down()[GLFW_MOUSE_BUTTON_RIGHT]; },
        "LeftUp", [&lua]() -> bool {
                Window &window = lua["__CurrentWindow"];
                return window.mouse_up()[GLFW_MOUSE_BUTTON_LEFT]; },
        "RightUp", [&lua]() -> bool {
                Window &window = lua["__CurrentWindow"];
                return window.mouse_up()[GLFW_MOUSE_BUTTON_RIGHT]; },
        "disable", [&lua](std::optional<bool> flag) {
            Window &window = lua["__CurrentWindow"];
            window.set_input_mode(GLFW_CURSOR, flag.value_or(true) ? GLFW_CURSOR_DISABLED : GLFW_CURSOR_NORMAL); });
}
