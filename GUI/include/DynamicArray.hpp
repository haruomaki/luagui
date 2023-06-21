#pragma once

#include <Window.hpp>

#include <Camera.hpp>

struct InterleavedVertexInfo2 {
    glm::vec3 coord_;
    RGBA color_;
};

class DynamicArray : public WorldObject, Draw {
    VertexArrayObject vao_;
    VertexBufferObject vbo_;
    const ProgramObject &shader_;
    const Camera &camera_;
    GLenum draw_mode_;
    size_t n_;
    size_t capacity_;
    static constexpr RGBA default_color{0.8, 0.8, 0.8, 1};

  public:
    vector<InterleavedVertexInfo2> vertices_;

    DynamicArray(const ProgramObject &shader, const Camera &camera, vector<glm::vec3> coords, vector<RGBA> colors = {}, GLenum draw_mode = GL_LINE_STRIP, GLenum usage = GL_DYNAMIC_DRAW)
        : shader_(shader)
        , camera_(camera)
        , draw_mode_(draw_mode)
        , n_(coords.size())
        , capacity_(coords.capacity()) {

        vector<InterleavedVertexInfo2> vers = {};
        for (size_t i = 0; i < n_; i++) {
            glm::vec3 coord = coords[i];
            RGBA color = (i < colors.size() ? colors[i] : default_color); // 色情報がないときは白色に
            vers.push_back({coord, color});
        }
        vertices_ = vers;

        // debug(MemoryView(reinterpret_cast<float *>(vers.data()), sizeof(InterleavedVertexInfo) / sizeof(float) * n_));

        // 空のVBOを生成
        vbo_ = VertexBufferObject::gen(sizeof(InterleavedVertexInfo2) * capacity_, nullptr, usage);

        // VAOを作成。頂点の座標と色を関連付ける
        vao_ = VertexArrayObject::gen();
        vao_.bind([&] {
            vbo_.bind([&] {
                shader.setAttribute("position", 3, GL_FLOAT, GL_FALSE, sizeof(InterleavedVertexInfo2), nullptr);                                  // 位置
                shader.setAttribute("color", 4, GL_FLOAT, GL_FALSE, sizeof(InterleavedVertexInfo2), reinterpret_cast<void *>(sizeof(float) * 3)); // 色 offset=12
            });
            getErrors();
        });
    }

    void draw() const override {
        // シェーダを有効化
        const auto &shader = shader_;
        shader.use();

        // ワールド座標変換
        glm::mat4 model_matrix = this->getAbsoluteTransform();

        // ビュー座標変換
        const glm::mat4 &view_matrix = camera_.getViewMatrix();

        // 合成して、モデルビュー行列を得る
        glm::mat4 model_view_matrix = view_matrix * model_matrix;
        shader.setUniform("modelViewMatrix", model_view_matrix);

        // モデルの描画
        shader.setUniform("is_tex", GL_FALSE);
        vao_.bind([&] {
            vbo_.bind([&] {
                glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(InterleavedVertexInfo2) * capacity_, vertices_.data());
            });
            glDrawArrays(draw_mode_, 0, GLsizei(n_));
        });
    }
};
