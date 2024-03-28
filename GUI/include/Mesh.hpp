#pragma once

#include "Draw.hpp"
#include "Material.hpp"
#include "ResourceUpdate.hpp"
#include <Shader.hpp>
#include <core.hpp>

// VRAMとの同期を毎フレーム自動で行わないメッシュ
class StaticMesh : virtual public Resource {
    friend class MeshObject;

  protected:
    const VertexArrayObject vao_;
    VertexBufferObject vbo_;
    ElementBufferObject ebo_;
    const Material &material_;
    const GLenum usage_;
    size_t n_ = 0;
    size_t indices_n_ = 0;
    size_t capacity_ = 0;
    size_t indices_capacity_ = 0;
    static constexpr RGBA default_color{0.8f, 0.8f, 0.8f, 1};

    void regenerate_vbo() {
        print("VBO生成");
        // 空のVBOを生成
        vbo_ = VertexBufferObject::gen(sizeof(InterleavedVertexInfo) * vertices.capacity(), nullptr, usage_);

        // VAOに頂点の座標と色を関連付ける
        vao_.bind([&] {
            vbo_.bind([&] {
                const auto &shader = this->material_.shader;
                shader.set_attribute("position", 3, GL_FLOAT, GL_FALSE, sizeof(InterleavedVertexInfo), nullptr);                                  // 位置
                shader.set_attribute("color", 4, GL_FLOAT, GL_FALSE, sizeof(InterleavedVertexInfo), reinterpret_cast<void *>(sizeof(float) * 3)); // 色 offset=12 NOLINT(performance-no-int-to-ptr)
                shader.set_attribute("uv", 2, GL_FLOAT, GL_FALSE, sizeof(InterleavedVertexInfo), reinterpret_cast<void *>(28));
            });
            getErrors();
        });
    }

    void regenerate_ibo() {
        print("IBO生成");
        this->ebo_ = ElementBufferObject::gen(sizeof(int) * indices.size(), indices.data(), usage_);
    }

  public:
    InterleavedVertexInfoVector vertices;
    std::vector<int> indices;

    StaticMesh(const Material *material = nullptr, const vector<glm::vec3> &coords = {}, const vector<RGBA> &colors = {}, GLenum usage = GL_STATIC_DRAW)
        : vao_(VertexArrayObject::gen())
        , material_(material == nullptr ? *this->get_window().default_material : *material)
        , usage_(usage)
        , n_(coords.size())
        , capacity_(coords.capacity()) {
        vector<InterleavedVertexInfo> vers = {};
        for (size_t i = 0; i < n_; i++) {
            glm::vec3 coord = coords[i];
            RGBA color = (i < colors.size() ? colors[i] : default_color); // 色情報がないときは白色に
            vers.push_back({coord, color});
        }
        vertices = vers;

        regenerate_vbo(); // VBOが常に存在するようにする
        regenerate_ibo();
    }

    void sync_vram() {
        // VBO,IBOの更新
        // verticesの長さ変更に伴ってヒープが再確保されるを検知し、新たにVBOを作り直す
        if (capacity_ != vertices.capacity()) {
            this->regenerate_vbo();
        }
        // インデックスも同様に更新
        if (indices_capacity_ != indices.capacity()) {
            this->regenerate_ibo();
        }

        // vectorの内容をVRAM上へコピー
        capacity_ = vertices.capacity();
        indices_capacity_ = indices.capacity();
        n_ = vertices.size();
        indices_n_ = indices.size();
        vbo_.subdata(0, sizeof(InterleavedVertexInfo) * capacity_, vertices.data());
        ebo_.subdata(0, sizeof(int) * indices.size(), indices.data());
    }
};

class Mesh : public StaticMesh, public ResourceUpdate {
    void update() override {
        sync_vram();
    }

  public:
    Mesh(const Material *material = nullptr, const vector<glm::vec3> &coords = {}, const vector<RGBA> &colors = {})
        : StaticMesh(material, coords, colors, GL_DYNAMIC_DRAW) {}
};

// template <GLenum usage>
// class Mesh : public StaticMesh {};
// template <>
// class Mesh<GL_DYNAMIC_DRAW> : public DynamicMesh {};

// class MeshCommon : public Resource {
//     VertexArrayObject vao_;
//     VertexBufferObject vbo_;
//     const Material &material_;
//     GLuint tex_id_ = 0;
//     const size_t n_; // TODO: GLsizeiにすればキャストを減らせる？
//     static constexpr RGBA default_color{0.8f, 0.8f, 0.8f, 1};

//     friend class MeshObject;

//   public:
//     MeshCommon(const Material &material, vector<glm::vec3> coords, vector<RGBA> colors = {}, vector<glm::vec2> uvs = {}, GLuint tex_id = 0, GLenum usage = GL_STATIC_DRAW)
//         : material_(material)
//         , tex_id_(tex_id)
//         , n_(coords.size()) {

//         vector<InterleavedVertexInfo> vers = {};
//         for (size_t i = 0; i < n_; i++) {
//             glm::vec3 coord = coords[i];
//             RGBA color = (i < colors.size() ? colors[i] : default_color); // 色情報がないときは白色に
//             glm::vec2 uv = (i < uvs.size() ? uvs[i] : glm::vec2{0, 0});   // uv情報がないときは(0,0)に
//             vers.push_back({coord, color, uv});
//         }

//         // debug(MemoryView(reinterpret_cast<float *>(vers.data()), sizeof(InterleavedVertexInfo) / sizeof(float) * n_));

//         // 頂点バッファオブジェクト（VBO）の生成とデータの転送
//         vbo_ = VertexBufferObject::gen(sizeof(InterleavedVertexInfo) * n_, vers.data(), usage);

//         // VAOを作成。頂点の座標と色、uvを関連付ける
//         vao_ = VertexArrayObject::gen();
//         vao_.bind([&] {
//             vbo_.bind([&] {
//                 const auto &shader = this->material_.shader;
//                 shader.set_attribute("position", 3, GL_FLOAT, GL_FALSE, sizeof(InterleavedVertexInfo), nullptr);                                    // 位置
//                 shader.set_attribute("color", 4, GL_FLOAT, GL_FALSE, sizeof(InterleavedVertexInfo), reinterpret_cast<void *>(sizeof(GLfloat) * 3)); // 色 offset=12 NOLINT(performance-no-int-to-ptr)
//                 getErrors();
//                 shader.set_attribute("uv", 2, GL_FLOAT, GL_FALSE, sizeof(InterleavedVertexInfo), reinterpret_cast<void *>(sizeof(glm::vec3) + sizeof(RGBA))); // uv座標 offset=28 NOLINT(performance-no-int-to-ptr)
//                 getErrors();
//             });
//         });
//     }
// };

class MeshObject : public Draw {
  public:
    StaticMesh &mesh;
    bool use_index;
    GLenum draw_mode = GL_LINE_STRIP;
    GLfloat point_size = 4;
    GLfloat line_width = 4;

    template <class Msh>
        requires std::is_convertible_v<Msh &, StaticMesh &>
    MeshObject(Msh &mesh, bool use_index = false)
        : mesh(mesh)
        , use_index(use_index) {}

    void draw(const Camera &camera) const override {
        // シェーダを有効化
        const auto &shader = this->mesh.material_.shader;
        shader.use();

        // 点の大きさ・線の太さを設定
        switch (draw_mode) {
        case GL_POINTS:
            glPointSize(point_size);
            break;
        case GL_LINES:
        case GL_LINE_STRIP:
        case GL_LINE_LOOP:
            glLineWidth(line_width);
            break;
        }

        // ワールド座標変換
        const glm::mat4 &model_matrix = this->get_absolute_transform();

        // ビュー座標変換
        const glm::mat4 &view_matrix = camera.get_view_matrix();

        // 合成して、モデルビュー行列を得る
        const auto &model_view_matrix = view_matrix * model_matrix;
        shader.set_uniform("modelViewMatrix", model_view_matrix);

        // 射影変換行列
        const auto projection_matrix = camera.get_projection_matrix();
        shader.set_uniform("projectionMatrix", projection_matrix);

        // モデルの描画
        shader.set_uniform("is_tex", GL_FALSE);
        this->mesh.vao_.bind([&] {
            if (use_index) {
                size_t indices_length = this->mesh.indices_n_;
                this->mesh.ebo_.bind([&] {
                    glDrawElements(draw_mode, GLsizei(sizeof(int) * indices_length), GL_UNSIGNED_INT, nullptr);
                });
            } else {
                size_t vertices_length = this->mesh.n_;
                glDrawArrays(draw_mode, 0, GLsizei(vertices_length));
            }
        });
    }
};
