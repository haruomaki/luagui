#include <SizeCallback.hpp>

SizeCallback::SizeCallback(GUI &window)
    : window_(window) {
    size_callback_ = [this](int width, int height) { this->sizeCallback(width, height); };
    window.size_callbacks_.insert(&size_callback_);
}
SizeCallback::~SizeCallback() {
    window_.size_callbacks_.erase(&size_callback_);
}
