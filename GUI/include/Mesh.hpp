#pragma once

#include "Camera.hpp"
#include "Draw.hpp"
#include "Material.hpp"
#include "ResourceUpdate.hpp"
#include "core.hpp"

// VRAMとの同期を毎フレーム自動で行わないメッシュ
class StaticMesh : virtual public Resource {
    friend class MeshObject;
    friend struct MeshDrawManager;

  protected:
    bool vao_should_regen_ = true;
    VertexBufferObject vbo_{};
    ElementBufferObject ebo_{};
    const GLenum usage_;
    size_t n_ = 0;
    size_t indices_n_ = 0;
    size_t capacity_ = 0;
    size_t indices_capacity_ = 0;
    static constexpr RGBA default_color{0.8f, 0.8f, 0.8f, 1};

    void regenerate_vbo() {
        print("VBO生成");
        // 空のVBOを生成
        this->vbo_ = VertexBufferObject(sizeof(InterleavedVertexInfo) * vertices.capacity(), nullptr, usage_);
        vao_should_regen_ = true;
    }

    void regenerate_ibo() {
        print("IBO生成");
        this->ebo_ = ElementBufferObject(sizeof(int) * indices.size(), indices.data(), usage_);
        vao_should_regen_ = true;
    }

  public:
    GLenum draw_mode;
    bool use_index = false;
    InterleavedVertexInfoVector vertices;
    std::vector<int> indices;

    StaticMesh(GLenum draw_mode = GL_TRIANGLE_STRIP, const vector<glm::vec3> &coords = {}, const vector<RGBA> &colors = {}, const vector<glm::vec2> &uvs = {}, GLenum usage = GL_STATIC_DRAW)
        : usage_(usage)
        , n_(coords.size())
        , capacity_(coords.capacity())
        , draw_mode(draw_mode) {
        vector<InterleavedVertexInfo> vers = {};
        for (size_t i = 0; i < n_; i++) {
            glm::vec3 coord = coords[i];
            RGBA color = (i < colors.size() ? colors[i] : default_color); // 色情報がないときは白色に
            glm::vec2 uv = (i < uvs.size() ? uvs[i] : glm::vec2{0, 0});   // uv情報がないときは(0,0)に
            vers.push_back({coord, color, uv});
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
    Mesh(GLenum draw_mode = GL_TRIANGLE_STRIP, const vector<glm::vec3> &coords = {}, const vector<RGBA> &colors = {}, const vector<glm::vec2> &uvs = {})
        : StaticMesh(draw_mode, coords, colors, uvs, GL_DYNAMIC_DRAW) {}
};

class MeshObject : public Draw {
  public:
    StaticMesh &mesh;
    Material *material;

    MeshObject(StaticMesh &mesh, Material *material = nullptr);

    void draw(const Camera &camera) const override;
};

struct MeshDrawManager {
    // メッシュ・マテリアル・シェーダの三項組ごとに一つモデル行列のvectorが決まる。
    // この行列キューごとに一回ドローコールを行う
    std::map<std::tuple<StaticMesh *, const Material *, const ProgramObject *>, vector<glm::mat4>> modelmatrices_storage{};
    // メッシュ・マテリアル・シェーダ・"モデル行列の生配列"の四項組ごとに一つVBO&VAOが決まる
    std::unordered_map<const glm::mat4 *, std::pair<VertexBufferObject, VertexArrayObject>> previous_vbovao{};

    static VertexArrayObject generate_vao(StaticMesh &mesh, const ProgramObject &shader, const VertexBufferObject &model_matrices_vbo) {
        print("VAO生成");
        auto vao = VertexArrayObject();

        // VAOに頂点の座標と色を関連付ける
        vao.bind([&] {
            mesh.vbo_.bind([&] {
                shader.set_attribute("position", 3, GL_FLOAT, GL_FALSE, sizeof(InterleavedVertexInfo), nullptr);                                  // 位置
                shader.set_attribute("color", 4, GL_FLOAT, GL_FALSE, sizeof(InterleavedVertexInfo), reinterpret_cast<void *>(sizeof(float) * 3)); // 色 offset=12 NOLINT(performance-no-int-to-ptr)
                shader.set_attribute("uv", 2, GL_FLOAT, GL_FALSE, sizeof(InterleavedVertexInfo), reinterpret_cast<void *>(28));
            });
            mesh.ebo_.keep_bind();
            model_matrices_vbo.bind([&] {
                shader.mat4_attribute("instanceModelMatrix");
            });
            getErrors();
        });

        mesh.vao_should_regen_ = false; // VAOの再生成が完了
        return vao;
    }

    static inline void draw_instanced(const StaticMesh &mesh, const Material &material, const VertexArrayObject &vao, size_t count_instances, const Camera &camera) {
        // シェーダを有効化
        const auto &shader = material.shader;
        shader.use();

        // 点の大きさ・線の太さを設定
        switch (mesh.draw_mode) {
        case GL_POINTS:
            glPointSize(GLfloat(material.point_size));
            break;
        case GL_LINES:
        case GL_LINE_STRIP:
        case GL_LINE_LOOP:
            glLineWidth(GLfloat(material.line_width));
            break;
        }

        // ワールド座標変換 = model_matrix
        // shader.set_uniform("modelMatrix", model_matrix);

        // ビュー座標変換
        const glm::mat4 &view_matrix = camera.get_view_matrix();
        shader.set_uniform("viewMatrix", view_matrix);

        // 射影変換行列
        const auto projection_matrix = camera.get_projection_matrix();
        shader.set_uniform("projectionMatrix", projection_matrix);

        // モデルの描画
        GLuint tex_id = material.texture.value_or(0);
        shader.set_uniform("is_tex", (tex_id == 0 ? GL_FALSE : GL_TRUE));
        vao.bind([&] {
            glBindTexture(GL_TEXTURE_2D, tex_id); // テクスチャを指定
            if (mesh.use_index) {
                size_t indices_length = mesh.indices_n_;
                glDrawElements(mesh.draw_mode, GLsizei(sizeof(int) * indices_length), GL_UNSIGNED_INT, nullptr);
            } else {
                size_t vertices_length = mesh.n_;
                glDrawArraysInstanced(mesh.draw_mode, 0, GLsizei(vertices_length), GLsizei(count_instances));
            }
            glBindTexture(GL_TEXTURE_2D, 0); // テクスチャのバインドを解除
        });
    }

    void register_to_draw(const MeshObject &obj) {
        auto &mesh = obj.mesh;
        const Material &material = *obj.material;
        const auto &shader = material.shader;
        StaticMesh *mp = &mesh;
        const Material *tp = &material; // = obj.material
        const ProgramObject *sp = &shader;
        const auto key = std::make_tuple(mp, tp, sp); // メッシュとシェーダの組をキーとする

        // 新規キーのとき
        if (!modelmatrices_storage.contains(key)) {
            modelmatrices_storage[key] = std::vector<glm::mat4>(0);
        }

        // モデル行列をキューに追加
        const glm::mat4 &model_matrix = obj.get_absolute_transform();
        modelmatrices_storage[key].push_back(model_matrix);
    }

    void draw_all_registered_objects(const Camera &camera) {
        std::unordered_map<const glm::mat4 *, std::pair<VertexBufferObject, VertexArrayObject>> current_vbovao{};

        // モデル行列キューの一覧を走査
        for (auto it = modelmatrices_storage.begin(); it != modelmatrices_storage.end();) {
            StaticMesh &mesh = *std::get<0>(it->first);
            const Material &material = *std::get<1>(it->first);
            auto &model_matrices = it->second; // モデル行列キュー

            // もはや使われなくなったモデル行列キューは削除し、次のキーへ
            if (model_matrices.size() == 0) {
                it = modelmatrices_storage.erase(it);
                continue;
            }

            VertexBufferObject model_matrices_vbo;
            VertexArrayObject batch_vao;

            const glm::mat4 *ptr = model_matrices.data(); // vector内部の生ポインタ
            if (previous_vbovao.contains(ptr) && !mesh.vao_should_regen_) {
                // 前回のキャッシュがあればそれを使用
                auto vv = std::move(previous_vbovao[ptr]);
                model_matrices_vbo = std::move(vv.first);
                batch_vao = std::move(vv.second);
            } else {
                // キャッシュが無い、もしくはメッシュのVBO更新等で再生成が必要ならば空のVBO&VAOを新規作成
                // TODO: メッシュのVBO更新の際はVAOの再生成だけで十分。model_matrices_vboの再生成は必要ない
                model_matrices_vbo = VertexBufferObject(sizeof(glm::mat4) * model_matrices.capacity(), nullptr, GL_DYNAMIC_DRAW);
                batch_vao = generate_vao(mesh, material.shader, model_matrices_vbo);
            }

            // VBOに毎フレーム値をコピー
            model_matrices_vbo.subdata(0, sizeof(glm::mat4) * model_matrices.size(), model_matrices.data());

            // 描画
            draw_instanced(mesh, material, batch_vao, model_matrices.size(), camera);

            // 描画を終えたモデル行列のキューは空に
            model_matrices.clear();
            current_vbovao[ptr] = std::make_pair(std::move(model_matrices_vbo), std::move(batch_vao));
            it++; // 忘れずに
        }

        // VBOとVAOのキャッシュを次のフレームに持ち越し
        previous_vbovao = std::move(current_vbovao); // VertexBufferObjectクラスがコピー不可なのでムーブ
    }
};
