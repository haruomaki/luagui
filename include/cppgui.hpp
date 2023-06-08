#pragma once

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

class Polygon {
    // GLuint vbo_;
    Window &window_;
    set<Polygon *> &polys_;
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

    ~Polygon() {
        polys_.erase(this);
        // key_callbacks_.erase(this->keyCallback);
    }

    void setTexture(GLuint tex_id) {
        tex_id_ = tex_id;
    }

    void draw() const;
};

class Window {
    GLFWwindow *gwin_;
    Rect<double> camera_;
    float zoom_ = 1;
    bool looping_ = false;
    set<Polygon *> polys_;
    set<function<void(int, int)> *> key_callbacks_;

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

    friend class Polygon;
};

GLuint createShader();
GLuint loadTexture(string filename);
