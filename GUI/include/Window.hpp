#pragma once

#include <graphical_base.hpp>

class GUI {
    GLFWwindow *gwin_;
    bool looping_ = false;
    std::set<std::function<void(int, int)> *> size_callbacks_;
    std::set<std::function<void(int, int)> *> key_callbacks_;
    std::set<std::function<void()> *> updates_;

  public:
    int tick = 0;

    GUI(int width, int height);
    ~GUI();
    void mainloop(const std::function<void()> &callback);
    [[nodiscard]] GLFWwindow *getGLFW() const;
    [[nodiscard]] pair<int, int> getWindowSize() const;
    [[nodiscard]] pair<int, int> getFrameBufferSize() const;
    [[nodiscard]] pair<float, float> getWindowContentScale() const;
    void close() const;

    // glfwGetKeyのラッパー。GLFW_PRESSのときtrue、GLFW_RELEASEのときfalse
    [[nodiscard]] bool getKey(int key) const;

    friend class SizeCallback;
    friend class KeyCallback;
    friend class Update;
};
