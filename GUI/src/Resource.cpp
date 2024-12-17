#include "Resource.hpp"

namespace {
Window *window_static = nullptr; // NOLINT(cppcoreguidelines-avoid-non-const-global-variables) コンストラクタを0引数にするためにグローバル変数経由で渡す
} // namespace

Window *Resource::get_window_static() {
    return window_static;
}

void Resource::set_window_static(Window *window) {
    window_static = window;
}
