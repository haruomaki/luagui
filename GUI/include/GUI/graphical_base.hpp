#pragma once

#define GLM_ENABLE_EXPERIMENTAL // 無いとglmが全く使えないが、他に対策があるかも＆アップデートで変わるかも

#include <GL/glew.h> // glとglfw3より早く

#include <GL/gl.h> // glGetString()のため
#include <GLFW/glfw3.h>
#include <glm/ext.hpp>

#include <iostream>

// base.hppのインクルードより先に書かないといけない処理
#include <SumiGL/logger.hpp>

// これより上に書かないと動かない
#include <GUI/base.hpp>

// base.hppのインクルードのあとでも問題無い処理
namespace base {

inline void get_errors_pre(const char *file, int line) {
    for (auto &e : GL::errors()) {
        print_headline("❎", file, line);
        std::cerr << " " << e.what() << std::endl;
    }
}

#ifdef DEBUG
#define getErrors() get_errors_pre(__FILE__, __LINE__) // NOLINT(cppcoreguidelines-macro-usage)
#else
#define getErrors()
#endif

// WARNING: Windows環境だとconstexprを付けられない
inline glm::mat4 TRANSLATE(glm::vec3 pos) { return glm::translate(glm::mat4(1), pos); }
inline glm::mat4 ROTATE(float angle, glm::vec3 axis) { return glm::rotate(glm::mat4(1), angle, axis); }
inline glm::mat4 SCALE(glm::vec3 scales) { return glm::scale(glm::mat4(1), scales); }
inline glm::quat ANGLE_X(float angle) { return glm::angleAxis(angle, glm::vec3{1, 0, 0}); }
inline glm::quat ANGLE_Y(float angle) { return glm::angleAxis(angle, glm::vec3{0, 1, 0}); }
inline glm::quat ANGLE_Z(float angle) { return glm::angleAxis(angle, glm::vec3{0, 0, 1}); }

// モデル行列（4x4）から平行移動成分・回転成分・拡縮成分を抽出する。
inline void decompose_transform(const glm::mat4 &transform, glm::vec3 &position, glm::quat &rotation, glm::vec3 &scale) {
    // 平行移動成分を取得
    position = glm::vec3(transform[3]);

    // スケール成分を取得
    scale = glm::vec3(
        glm::length(glm::vec3(transform[0])),
        glm::length(glm::vec3(transform[1])),
        glm::length(glm::vec3(transform[2])));

    // 回転成分を取得
    glm::mat3 rotation_matrix = glm::mat3(
        glm::vec3(transform[0]) / scale.x,
        glm::vec3(transform[1]) / scale.y,
        glm::vec3(transform[2]) / scale.z);
    rotation = glm::quat_cast(rotation_matrix);
}

} // namespace base

using namespace base;
