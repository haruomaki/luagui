#pragma once

#include <GUI.hpp>

class Window {
    GLFWwindow *gwin_;
    bool looping_ = false;

  public:
    int tick_ = 0;

    Window(int width, int height);
    ~Window();
    void mainloop(const std::function<void()> &callback);
    [[nodiscard]] pair<int, int> getWindowSize() const;
    [[nodiscard]] pair<int, int> getFrameBufferSize() const;
    [[nodiscard]] pair<float, float> getWindowContentScale() const;
};
