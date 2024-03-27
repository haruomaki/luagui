#include <WindowObject.hpp>

// SizeCallback::SizeCallback() {
//     size_callback_ = [this](int width, int height) { this->sizeCallback(width, height); };
//     this->window_->size_callbacks_.insert(&size_callback_);
// }
// SizeCallback::~SizeCallback() {
//     // FIXME: window_の参照が切れている場合メモリエラーが発生する
//     window_->size_callbacks_.erase(&size_callback_);
// }

namespace {
Window *window_static = nullptr; // NOLINT(cppcoreguidelines-avoid-non-const-global-variables) コンストラクタを0引数にするためにグローバル変数経由で渡す
} // namespace

Window *Resource::get_window_static() {
    return window_static;
}

void Resource::set_window_static(Window *window) {
    window_static = window;
}
