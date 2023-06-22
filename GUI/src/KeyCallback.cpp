#include <KeyCallback.hpp>

KeyCallback::KeyCallback(Window &window)
    : window_(window) {
    key_callback_ = [this](int key, int action) { this->keyCallback(key, action); };
    window.key_callbacks_.insert(&key_callback_);
}
KeyCallback::~KeyCallback() {
    window_.key_callbacks_.erase(&key_callback_);
}
