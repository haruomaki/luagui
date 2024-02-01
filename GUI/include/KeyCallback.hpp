#pragma once

#include <Window.hpp>

class KeyCallback {
    GUI &window_;
    std::function<void(int, int)> key_callback_;

  public:
    virtual void keyCallback(int key, int action) = 0;
    KeyCallback(GUI &window);
    virtual ~KeyCallback();
};
