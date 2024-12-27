#include "Resource.hpp"

namespace {
GUI *gui_static = nullptr; // NOLINT(cppcoreguidelines-avoid-non-const-global-variables) コンストラクタを0引数にするためにグローバル変数経由で渡す
} // namespace

GUI *Resource::get_gui_static() {
    return gui_static;
}

void Resource::set_gui_static(GUI *gui) {
    gui_static = gui;
}
