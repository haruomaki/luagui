#pragma once

#include <Drawable.hpp>
#include <Shader.hpp>
#include <core.hpp>

class Polygon {
    VertexArrayObject vao_;
    VertexBufferObject vbo_;
    const ProgramObject &shader_;
    GLuint tex_id_ = 0;
    const size_t n_; // TODO: GLsizeiにすればキャストを減らせる？
    static constexpr RGBA default_color{0.8f, 0.8f, 0.8f, 1};

    friend class Shape;

  public:
    Polygon(const ProgramObject &shader, vector<glm::vec3> coords, vector<RGBA> colors = {}, GLuint tex_id = 0, GLenum usage = GL_STATIC_DRAW)
        : shader_(shader)
        , tex_id_(tex_id)
        , n_(coords.size()) {

        vector<InterleavedVertexInfo> vers = {};
        for (size_t i = 0; i < n_; i++) {
            glm::vec3 coord = coords[i];
            RGBA color = (i < colors.size() ? colors[i] : default_color); // 色情報がないときは白色に
            vers.push_back({coord, color});
        }

        // debug(MemoryView(reinterpret_cast<float *>(vers.data()), sizeof(InterleavedVertexInfo) / sizeof(float) * n_));

        // 頂点バッファオブジェクト（VBO）の生成とデータの転送
        vbo_ = VertexBufferObject::gen(sizeof(InterleavedVertexInfo) * n_, vers.data(), usage);

        // VAOを作成。頂点の座標と色、uvを関連付ける
        vao_ = VertexArrayObject::gen();
        vao_.bind([&] {
            vbo_.bind([&] {
                shader.set_attribute("position", 3, GL_FLOAT, GL_FALSE, sizeof(InterleavedVertexInfo), nullptr);                                    // 位置
                shader.set_attribute("color", 4, GL_FLOAT, GL_FALSE, sizeof(InterleavedVertexInfo), reinterpret_cast<void *>(sizeof(GLfloat) * 3)); // 色 offset=12 NOLINT(performance-no-int-to-ptr)
                getErrors();
                shader.set_attribute("uv", 2, GL_FLOAT, GL_FALSE, 0, nullptr); // FIXME: uv座標をvbo経由で設定できるようにする
                getErrors();
            });
        });
    }
};

class Shape : public DrawableWorldObject {
    const Polygon &polygon_;

  public:
    Shape(World &world, const Polygon &polygon)
        : DrawableWorldObject(world)
        , polygon_(polygon) {}

    void draw(const Camera &camera) const override {
        // シェーダを有効化
        const auto &shader = polygon_.shader_;
        shader.use();

        // モデルビュー行列
        const auto model_matrix = this->getAbsoluteTransform();
        const auto view_matrix = camera.getViewMatrix();
        const auto model_view_matrix = view_matrix * model_matrix;
        shader.set_uniform("modelViewMatrix", model_view_matrix);

        // 射影変換行列
        const auto projection_matrix = camera.getProjectionMatrix();
        shader.set_uniform("projectionMatrix", projection_matrix);

        // モデルの描画
        shader.set_uniform("is_tex", (polygon_.tex_id_ != 0 ? GL_TRUE : GL_FALSE));
        polygon_.vao_.bind([&] {
            glBindTexture(GL_TEXTURE_2D, polygon_.tex_id_);
            glDrawArrays(GL_TRIANGLE_FAN, 0, GLsizei(polygon_.n_));
            glBindTexture(GL_TEXTURE_2D, 0);
        });
    }
};
