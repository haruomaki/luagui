#pragma once

#include <GUI.hpp>

class Window {
    GLFWwindow *gwin_;
    bool looping_ = false;
    std::set<std::function<void(int, int)> *> size_callbacks_;
    std::set<std::function<void(int, int)> *> key_callbacks_;
    std::set<std::function<void()> *> updates_;

  public:
    int tick_ = 0;

    Window(int width, int height);
    ~Window();
    void mainloop(const std::function<void()> &callback);
    [[nodiscard]] pair<int, int> getWindowSize() const;
    [[nodiscard]] pair<int, int> getFrameBufferSize() const;
    [[nodiscard]] pair<float, float> getWindowContentScale() const;

    friend class SizeCallback;
    friend class KeyCallback;
    friend class Update;
};
