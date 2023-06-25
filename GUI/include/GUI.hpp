#pragma once

#include <GL/glew.h> // glとglfw3より早く

#include <GL/gl.h> // glGetString()のため
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>
#include <glm/gtx/string_cast.hpp>

#include <iostream>

// base.hppのインクルードより先に書かないといけない処理
using namespace std;

namespace base {

template <glm::length_t L, class T, glm::qualifier Q>
ostream &operator<<(ostream &os, const glm::vec<L, T, Q> input_vec) {
    os << glm::to_string(input_vec);
    return os;
}

template <glm::length_t C, glm::length_t R, class T, glm::qualifier Q>
ostream &operator<<(ostream &os, const glm::mat<C, R, T, Q> input_mat) {
    os << glm::to_string(input_mat);
    return os;
}

} // namespace base

// これより上に書かないと動かない
#include <base.hpp>

// base.hppのインクルードのあとでも問題無い処理
namespace base {

inline void getErrors() {
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        debug(err);
    }
}

struct RGB {
    unsigned char r_, g_, b_;
};

struct RGBA {
    float r_, g_, b_, a_;

    operator glm::vec4() const {
        return {r_, g_, b_, a_};
    }
};

#define TRANSLATE(...) glm::translate(glm::mat4(1), __VA_ARGS__)
#define ROTATE(angle, ...) glm::rotate(glm::mat4(1), angle, __VA_ARGS__)
#define ANGLE_X(angle) glm::angleAxis(angle, glm::vec3{1, 0, 0})
#define ANGLE_Y(angle) glm::angleAxis(angle, glm::vec3{0, 1, 0})
#define ANGLE_Z(angle) glm::angleAxis(angle, glm::vec3{0, 0, 1})
#define SCALE(...) glm::scale(glm::mat4(1), __VA_ARGS__)

} // namespace base

using namespace base;
