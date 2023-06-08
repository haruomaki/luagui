#pragma once

#include <Draw.hpp>
#include <KeyCallback.hpp>
#include <Update.hpp>
#include <guibase.hpp>

#include <fstream>
#include <optional>

class Window;

template <typename T>
struct Rect {
    T left_, right_, bottom_, top_;
};

class Polygon : Draw {
    // GLuint vbo_;
    Window &window_;
    GLuint tex_id_ = 0;

  protected:
    vector<Point<GLfloat>> vertices_;

  public:
    vector<RGBA> colors_;
    // function<void(int, int)> key_callback_ = [](int, int) {
    //     cout << "💙💙💙💙💙" << endl;
    // };

    Polygon(Window &window, vector<Point<float>> points, GLenum usage = GL_DYNAMIC_DRAW);
    // Polygon(Window &window) {}
    // Polygon(Window &window, vector<Point<float>> ps) {}

    void setTexture(GLuint tex_id) {
        tex_id_ = tex_id;
    }

    void draw() const override;
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
