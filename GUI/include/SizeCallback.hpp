#pragma once

// #include <Window.hpp>
#include <graphical_base.hpp>

class Window;

class SizeCallback {
    friend class Window;
    // Window *window_ = nullptr;

  public:
    std::function<void(SizeCallback *this_ptr, int width, int height)> size_callback;

    SizeCallback() = default;
    virtual ~SizeCallback() = default; // virtualにするのが重要
    SizeCallback(const SizeCallback &) = default;
    SizeCallback &operator=(const SizeCallback &) = default;
    SizeCallback(SizeCallback &&) = default;
    SizeCallback &operator=(SizeCallback &&) = default;
};
