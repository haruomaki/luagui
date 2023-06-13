#pragma once

#include <cppgui.hpp>

struct InterleavedVertexInfo {
    Point<float> coord_;
    RGBA color_;
};

class Polygon : Draw {
    GLuint vbo_;
    Window &window_;
    GLuint tex_id_ = 0;
    vector<InterleavedVertexInfo> vers_ = {};
    const size_t n_;

  public:
    Polygon(Window &window, vector<Point<float>> coords, vector<RGBA> colors = {}, GLuint tex_id = 0, GLenum usage = GL_DYNAMIC_DRAW)
        : window_(window)
        , tex_id_(tex_id)
        , n_(coords.size()) {
        for (size_t i = 0; i < n_; i++) {
            Point<float> coord = coords[i];
            RGBA color = (i < colors.size() ? colors[i] : RGBA{0.8, 0.8, 0.8, 1});
            vers_.push_back({coord, color});
        }

        // 頂点バッファオブジェクト（VBO）の生成とデータの転送
        glGenBuffers(1, &vbo_);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_);
        glBufferData(GL_ARRAY_BUFFER, sizeof(InterleavedVertexInfo) * n_, vers_.data(), usage); // WARNING: coords_のアライメントによっては動作しない
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    void draw() const override {
        // 何番目のattribute変数か
        int positionLocation = glGetAttribLocation(window_.program_id_, "position");
        int uvLocation = glGetAttribLocation(window_.program_id_, "uv");
        int colorLocation = glGetAttribLocation(window_.program_id_, "color");

        int textureLocation = glGetUniformLocation(window_.program_id_, "texture");
        int is_texLocation = glGetUniformLocation(window_.program_id_, "is_tex");

        // テクスチャを持つ場合
        if (tex_id_ != 0) {
        } else {
            // attribute属性を有効にする
            glEnableVertexAttribArray(positionLocation);
            glEnableVertexAttribArray(colorLocation);

            // attribute属性を登録
            glBindBuffer(GL_ARRAY_BUFFER, vbo_);
            glVertexAttribPointer(positionLocation, 2, GL_FLOAT, false, sizeof(InterleavedVertexInfo), nullptr);
            glVertexAttribPointer(colorLocation, 4, GL_FLOAT, false, sizeof(InterleavedVertexInfo), reinterpret_cast<void *>(sizeof(Point<float>)));
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            // モデルの描画
            glUniform1i(is_texLocation, int(false));
            glDrawArrays(GL_TRIANGLE_FAN, 0, n_);

            // // glDisableVertexAttribArray(0); // TODO: 不要？
        }
    }
};
