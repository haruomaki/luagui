#pragma once

#include <cppgui.hpp>

struct InterleavedVertexInfo {
    glm::vec3 coord_;
    RGBA color_;
};

class Polygon {
    GLuint vbo_, vao_;
    Window &window_;
    GLuint tex_id_ = 0;
    const size_t n_;
    static constexpr GLfloat vertex_uv[4][2] = {{1, 0}, {0, 0}, {0, 1}, {1, 1}};
    static constexpr RGBA default_color{0.8, 0.8, 0.8, 1};

    friend class PolygonInstance;

  public:
    Polygon(Window &window, vector<Point<float>> coords, vector<RGBA> colors = {}, GLuint tex_id = 0, GLenum usage = GL_STATIC_DRAW)
        : window_(window)
        , tex_id_(tex_id)
        , n_(coords.size()) {

        vector<InterleavedVertexInfo> vers = {};
        for (size_t i = 0; i < n_; i++) {
            glm::vec3 coord = {coords[i].x_, coords[i].y_, 1};
            RGBA color = (i < colors.size() ? colors[i] : default_color); // 色情報がないときは白色に
            vers.push_back({coord, color});
        }

        // debug(MemoryView(reinterpret_cast<float *>(vers.data()), sizeof(InterleavedVertexInfo) / sizeof(float) * n_));

        int va_position_location = glGetAttribLocation(window_.program_id_, "position");
        int va_color_location = glGetAttribLocation(window_.program_id_, "color");
        int uv_location = glGetAttribLocation(window_.program_id_, "uv");

        // 頂点バッファオブジェクト（VBO）の生成とデータの転送
        glGenBuffers(1, &vbo_);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_);
        glBufferData(GL_ARRAY_BUFFER, sizeof(InterleavedVertexInfo) * n_, vers.data(), usage); // WARNING: versのアライメントによっては動作しない

        // VAOを作成。頂点の座標と色、uvを関連付ける
        glGenVertexArrays(1, &vao_); // VAOの生成
        glBindVertexArray(vao_);     // VAOをバインド

        glEnableVertexAttribArray(va_position_location);
        glEnableVertexAttribArray(va_color_location);
        glVertexAttribPointer(va_position_location, 3, GL_FLOAT, GL_FALSE, sizeof(InterleavedVertexInfo), nullptr);                                  // 位置
        glVertexAttribPointer(va_color_location, 4, GL_FLOAT, GL_FALSE, sizeof(InterleavedVertexInfo), reinterpret_cast<void *>(sizeof(float) * 3)); // 色 offset=12

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // VBOのバインドを解除

        // uvの設定
        glEnableVertexAttribArray(uv_location);
        glVertexAttribPointer(uv_location, 2, GL_FLOAT, GL_FALSE, 0, vertex_uv);

        glBindVertexArray(0);
        // VAOのバインドを解除
    }
};

class PolygonInstance : Draw, public WorldObject {
    const Polygon &polygon_;

  public:
    PolygonInstance(const Polygon &polygon)
        : polygon_(polygon) {}

    void draw() const override {
        int is_tex_location = glGetUniformLocation(polygon_.window_.program_id_, "is_tex");
        int model_view_matrix_location = glGetUniformLocation(polygon_.window_.program_id_, "modelViewMatrix");

        // ワールド座標変換
        auto diff = this->getAbsolutePosition();
        glm::mat4 model_matrix = glm::translate(glm::mat4(1), glm::vec3(diff.x_, diff.y_, 0));

        // ビュー座標変換
        const glm::mat4 &view_matrix = polygon_.window_.getViewMatrix();

        // 合成して、モデルビュー行列を得る
        glm::mat4 model_view_matrix = view_matrix * model_matrix;
        glUniformMatrix4fv(model_view_matrix_location, 1, GL_FALSE, glm::value_ptr(model_view_matrix));

        // モデルの描画
        glUniform1i(is_tex_location, (polygon_.tex_id_ != 0 ? GL_TRUE : GL_FALSE));
        glBindVertexArray(polygon_.vao_);
        glBindTexture(GL_TEXTURE_2D, polygon_.tex_id_);
        glDrawArrays(GL_TRIANGLE_FAN, 0, polygon_.n_);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindVertexArray(0); // NOTE: 必須
    }
};
