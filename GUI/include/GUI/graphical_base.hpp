#pragma once

#define GLM_ENABLE_EXPERIMENTAL // 無いとglmが全く使えないが、他に対策があるかも＆アップデートで変わるかも

#include <GL/glew.h> // glとglfw3より早く

#include <GL/gl.h> // glGetString()のため
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>

#include <iostream>

// base.hppのインクルードより先に書かないといけない処理
#include <logger.hpp>

// これより上に書かないと動かない
#include <GUI/base.hpp>

// base.hppのインクルードのあとでも問題無い処理
namespace base {

inline void get_errors_pre(const char *file, int line) {
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR) {
        // print_headline("❎", file, line);
        print_headline("cr", file, line);
        std::cerr << " " << err << std::endl;
    }
}

#ifdef DEBUG
#define getErrors() get_errors_pre(__FILE__, __LINE__) // NOLINT(cppcoreguidelines-macro-usage)
#else
#define getErrors()
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

// WARNING: Windows環境だとconstexprを付けられない
inline glm::mat4 TRANSLATE(glm::vec3 pos) { return glm::translate(glm::mat4(1), pos); }
inline glm::mat4 ROTATE(float angle, glm::vec3 axis) { return glm::rotate(glm::mat4(1), angle, axis); }
inline glm::mat4 SCALE(glm::vec3 scales) { return glm::scale(glm::mat4(1), scales); }
inline glm::quat ANGLE_X(float angle) { return glm::angleAxis(angle, glm::vec3{1, 0, 0}); }
inline glm::quat ANGLE_Y(float angle) { return glm::angleAxis(angle, glm::vec3{0, 1, 0}); }
inline glm::quat ANGLE_Z(float angle) { return glm::angleAxis(angle, glm::vec3{0, 0, 1}); }

} // namespace base

using namespace base;
