#pragma once

// #include <Window.hpp>
#include <graphical_base.hpp>

class Window;

class SizeCallback {
    friend class Window;
    Window *window_ = nullptr;
    Window &getWindow() const {
        return *this->window_;
    }

  public:
    std::function<void(SizeCallback *, int, int)> size_callback;

    // SizeCallback();
    // virtual ~SizeCallback();
    // virtual void sizeCallback(int width, int height) = 0;
    virtual ~SizeCallback() {
        print("SizeCallbackのデストラクタです");
    }
};
