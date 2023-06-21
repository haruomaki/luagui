#include <SizeCallback.hpp>

static std::set<std::function<void(int, int)> *> global_size_callbacks;

SizeCallback::SizeCallback() {
    size_callback_ = [this](int width, int height) { this->sizeCallback(width, height); };
    global_size_callbacks.insert(&size_callback_);
}
SizeCallback::~SizeCallback() {
    global_size_callbacks.erase(&size_callback_);
}

void masterSizeCallback(GLFWwindow * /*gwin*/, int width, int height) {
    for (auto *size_callback : global_size_callbacks) {
        (*size_callback)(width, height);
    }
}
