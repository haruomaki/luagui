#pragma once

#include <Draw.hpp>
#include <KeyCallback.hpp>
#include <Polygon.hpp>
#include <Update.hpp>
#include <guibase.hpp>

#include <fstream>
#include <optional>

template <typename T>
struct Rect {
    T left_, right_, bottom_, top_;
};

class Window {
    GLFWwindow *gwin_;
    Rect<double> camera_;
    float zoom_ = 1;
    bool looping_ = false;

  public:
    int tick_ = 0;
    GLuint program_id_;

    Window(int width, int height);
    ~Window();
    void mainloop(std::function<void()> f);
    pair<int, int> getWindowSize();
    pair<int, int> getFrameBufferSize();
    pair<float, float> getWindowContentScale();
    void setCamera(Point<float> pos, float zoom);
    void setCameraCorner(Point<float> pos, float zoom);
};

GLuint createShader();
GLuint loadTexture(string filename);
