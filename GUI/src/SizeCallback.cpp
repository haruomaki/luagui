#include <SizeCallback.hpp>

SizeCallback::SizeCallback(Window &window)
    : window_(window) {
    size_callback_ = [this](int width, int height) { this->sizeCallback(width, height); };
    window.size_callbacks_.insert(&size_callback_);
}
SizeCallback::~SizeCallback() {
    // FIXME: window_の参照が切れている場合メモリエラーが発生する
    window_.size_callbacks_.erase(&size_callback_);
}
