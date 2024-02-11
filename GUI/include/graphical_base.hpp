#pragma once

#define GLM_ENABLE_EXPERIMENTAL // TODO: 無いとglmが全く使えないが、他に対策があるかも＆アップデートで変わるかも

#include <GL/glew.h> // glとglfw3より早く

#include <GL/gl.h> // glGetString()のため
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>
#include <glm/gtx/string_cast.hpp>

#include <iostream>

// base.hppのインクルードより先に書かないといけない処理
namespace base {

template <glm::length_t l, class T, glm::qualifier q>
std::ostream &operator<<(std::ostream &os, const glm::vec<l, T, q> input_vec) {
    os << glm::to_string(input_vec);
    return os;
}

template <glm::length_t c, glm::length_t r, class T, glm::qualifier q>
std::ostream &operator<<(std::ostream &os, const glm::mat<c, r, T, q> input_mat) {
    os << glm::to_string(input_mat);
    return os;
}

} // namespace base

// これより上に書かないと動かない
#include <base.hpp>

// base.hppのインクルードのあとでも問題無い処理
namespace base {

inline void get_errors_pre(const char *file, int line) {
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        print_headline("❎", file, line);
        std::cerr << " " << err << std::endl;
    }
}

#ifdef DEBUG
#define getErrors() get_errors_pre(__FILE__, __LINE__) // NOLINT(cppcoreguidelines-macro-usage)
#else
#defile getErrors()
#endif

struct RGB {
    unsigned char r, g, b;
};

struct RGBA {
    float r, g, b, a;

    operator glm::vec4() const {
        return {r, g, b, a};
    }
};

// TODO: 可変長テンプレート関数化する
#define TRANSLATE(...) glm::translate(glm::mat4(1), __VA_ARGS__)
#define ROTATE(angle, ...) glm::rotate(glm::mat4(1), angle, __VA_ARGS__)
#define ANGLE_X(angle) glm::angleAxis(angle, glm::vec3{1, 0, 0})
#define ANGLE_Y(angle) glm::angleAxis(angle, glm::vec3{0, 1, 0})
#define ANGLE_Z(angle) glm::angleAxis(angle, glm::vec3{0, 0, 1})
#define SCALE(...) glm::scale(glm::mat4(1), __VA_ARGS__)

} // namespace base

using namespace base;
