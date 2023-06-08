#pragma once

#include <Draw.hpp>

class Window;

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
