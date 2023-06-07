#include <KeyCallback.hpp>

static std::set<std::function<void(int, int)> *> global_key_callbacks;

KeyCallback::KeyCallback() {
    key_callback_ = bind(&KeyCallback::keyCallback, this, std::placeholders::_1, std::placeholders::_2);
    global_key_callbacks.insert(&key_callback_);
}
KeyCallback::~KeyCallback() {
    global_key_callbacks.erase(&key_callback_);
}

void masterKeyCallback(GLFWwindow *gwin, int key, int scancode, int action, int mods) {
    for (auto key_callback : global_key_callbacks) {
        (*key_callback)(key, action);
    }
}
