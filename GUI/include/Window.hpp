#pragma once

#include <graphical_base.hpp>

class Window {
    GLFWwindow *gwin_ = nullptr;
    std::set<std::function<void(int, int)> *> size_callbacks_;
    std::set<std::function<void(int, int)> *> key_callbacks_;
    std::set<std::function<void()> *> updates_;

    friend class GUI;
    friend class SizeCallback;
    friend class KeyCallback;
    friend class Update;

  public:
    Window(int width, int height, const char *title);
    // ~Window();

    // // コピーは禁止する
    // Window(const Window &) = default;
    // Window &operator=(const Window &) = default;

    // // ムーブコンストラクタはデフォルト、ムーブ代入は禁止
    // Window(Window &&) = default;
    // Window &operator=(Window &&) = default;

    [[nodiscard]] GLFWwindow *getGLFW() const;
    [[nodiscard]] pair<int, int> getWindowSize() const;
    [[nodiscard]] pair<int, int> getFrameBufferSize() const;
    [[nodiscard]] pair<float, float> getWindowContentScale() const;
    void close() const;

    // glfwGetKeyのラッパー。GLFW_PRESSのときtrue、GLFW_RELEASEのときfalse
    [[nodiscard]] bool getKey(int key) const;
};
