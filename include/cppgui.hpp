#pragma once

#include <KeyCallback.hpp>
#include <guibase.hpp>

class Window;

template <typename T>
struct Rect {
    T left_, right_, bottom_, top_;
};

class Polygon {
    vector<Point<GLfloat>> vertices_;
    GLuint vbo_;
    Window &window_;
    set<Polygon *> &polys_;

  public:
    RGB color_;
    function<void(int, int)> key_callback_ = nullptr;

    Polygon(Window &window, vector<Point<float>> points, GLenum usage = GL_DYNAMIC_DRAW);

    ~Polygon() {
        polys_.erase(this);
        // key_callbacks_.erase(this->keyCallback);
    }

    void draw() const {
        glBindBuffer(GL_ARRAY_BUFFER, vbo_);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
        glColor3ub(color_.r_, color_.b_, color_.g_);
        glDrawArrays(GL_POLYGON, 0, vertices_.size());

        glDisableVertexAttribArray(0); // TODO: 不要？
    }

    void update() {
        for (auto &vert : vertices_) {
            const float velocity = 0.3f;
            vert.y_ += velocity;
        }

        // VRAM上のVBOを更新
        glBindBuffer(GL_ARRAY_BUFFER, vbo_);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(GLfloat) * vertices_.size() * 2, vertices_.data());
    }
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
