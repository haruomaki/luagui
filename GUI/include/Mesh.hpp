#pragma once

#include "Camera.hpp"
#include "Draw.hpp"
#include "Material.hpp"
#include "ResourceUpdate.hpp"
#include "core.hpp"

#include <algorithm>
#include <map>

// VRAMとの同期を毎フレーム自動で行わないメッシュ
class StaticMesh : virtual public Resource {
    friend class MeshObject;
    friend struct MeshDrawManager;

  protected:
    bool vao_should_regen_ = true;
    VertexBufferObject vbo_;
    ElementBufferObject ebo_;
    const GLenum usage_;
    size_t n_ = 0;
    size_t indices_n_ = 0;
    size_t capacity_ = 0;
    size_t indices_capacity_ = 0;

    void regenerate_vbo() {
        info("VBO生成");
        // 空のVBOを生成
        this->vbo_ = VertexBufferObject(sizeof(InterleavedVertexInfo) * vertices.capacity(), nullptr, usage_);
        vao_should_regen_ = true;
    }

    void regenerate_ibo() {
        info("IBO生成");
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
        vector<InterleavedVertexInfo> vers(n_);
        for (size_t i = 0; i < n_; i++) {
            vers[i].coord = coords[i];
            if (i < colors.size()) vers[i].color = colors[i]; // 色情報がないときは白色に
            if (i < uvs.size()) vers[i].uv = uvs[i];          // uv情報がないときは(0,0)に
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

class MeshObject : virtual public WorldObject {
  public:
    StaticMesh &mesh;
    Material &material;

    MeshObject(StaticMesh &mesh, Material *material = nullptr);
    ~MeshObject() override;
};

struct ModelMatricesObservation {
    vector<glm::mat4> model_matrices;
    std::unordered_map<const MeshObject *, size_t> object_index_map;
    std::vector<const MeshObject *> initial_list;
    std::vector<const MeshObject *> delete_list;

    // メッシュ・マテリアル・シェーダ・"モデル行列の生配列"の四項組ごとに一つVBO&VAOが決まる
    const glm::mat4 *matrices_raw = nullptr;
    std::pair<VertexBufferObject, VertexArrayObject> vbovao;
};

struct MeshDrawManager {
    // メッシュ・マテリアル・シェーダの三項組ごとに一つモデル行列のvectorが決まる。
    using KeyType = std::tuple<StaticMesh *, const Material *, const ProgramObject *>;
    // この行列キューごとに一回ドローコールを行う
    std::map<KeyType, ModelMatricesObservation> observations;

    static inline KeyType key_from(const MeshObject *obj) {
        StaticMesh *mesh = &obj->mesh;
        const Material *material = &obj->material;
        const ProgramObject *shader = &obj->material.shader;
        auto key = std::make_tuple(mesh, material, shader);
        return key;
    }

    void set_model_matrix(const MeshObject *obj) {
        auto key = key_from(obj);

        if (observations.contains(key)) {
            auto &obj_ix_map = observations[key].object_index_map;

            if (obj_ix_map.contains(obj)) {
                // すでに登録済みのメッシュオブジェクトの場合、ただちに書き換え
                size_t index = obj_ix_map[obj];
                const auto &model_matrix = obj->get_absolute_transform();
                observations[key].model_matrices[index] = model_matrix;
            } else {
                // 新たなメッシュオブジェクトの場合、一旦initial_valuesに蓄えておく
                observations[key].initial_list.emplace_back(obj);
            }
        } else {
            // そもそも初めてのkeyのオブジェクトだった場合、キーを追加してやはりinitial_valuesに蓄える
            observations[key] = ModelMatricesObservation{};
            observations[key].initial_list.emplace_back(obj);
        }
    }

    void delete_model_matrix(const MeshObject *obj) {
        auto key = key_from(obj);
        assert(observations.contains(key)); // 一度も登録されていないキーを持つオブジェクトの削除要求
        observations[key].delete_list.push_back(obj);
    }

    void step() {
        // initial_listとdelete_listのいずれかが空でないすべてのキーについてモデル行列キューを再生成
        for (auto &[key, obs] : observations) {
            if (obs.initial_list.size() != 0 || obs.delete_list.size() != 0) {
                auto &obj_ix_map = obs.object_index_map;

                // 登録予定キーを追加
                for (const auto *ptr : obs.initial_list) {
                    obj_ix_map.insert_or_assign(ptr, 0); // ダミーの値を入れておく
                }

                // 削除予定キーを削除
                for (const auto *ptr : obs.delete_list) {
                    obj_ix_map.erase(ptr);
                }

                // インデックスの振り直しとモデル行列キューの再生成を一挙に行う
                auto queue_size = obj_ix_map.size();
                obs.model_matrices = std::vector<glm::mat4>(queue_size); // モデル行列キュー再生成
                size_t counter = 0;
                for (auto &[obj, index] : obj_ix_map) {
                    const auto &model_matrix = obj->get_absolute_transform();
                    index = counter++;
                    obs.model_matrices[index] = model_matrix;
                }
            }

            // 各キューを削除
            obs.initial_list = std::vector<const MeshObject *>();
            obs.delete_list = std::vector<const MeshObject *>();
        }

        // 使われなくなったモデル行列キューは削除する
        std::erase_if(observations, [](const auto &item) {
            const auto &[key, obs] = item;
            return (obs.model_matrices.size() == 0); // 必ず obs.object_index_map == 0 でもあるはず
        });
    }

    static VertexArrayObject generate_vao(StaticMesh &mesh, const ProgramObject &shader, const VertexBufferObject &model_matrices_vbo) {
        info("VAO生成");
        auto vao = VertexArrayObject();

        // VAOに頂点の座標と色を関連付ける
        vao.bind([&] {
            mesh.vbo_.bind([&] {
                shader.set_attribute_float("position", 3, false, sizeof(InterleavedVertexInfo), nullptr);                                  // 位置
                shader.set_attribute_float("color", 4, false, sizeof(InterleavedVertexInfo), reinterpret_cast<void *>(sizeof(float) * 3)); // 色 offset=12 NOLINT(performance-no-int-to-ptr)
                shader.set_attribute_float("uv", 2, false, sizeof(InterleavedVertexInfo), reinterpret_cast<void *>(28));
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
        default: // GL_TRIANGLE_FANなどのときは何もしない
        }

        // ワールド座標変換 = instanceModelMatrix

        // ビュー座標変換
        const glm::mat4 &view_matrix = camera.get_view_matrix();
        shader.set_uniform("viewMatrix", view_matrix);

        // 射影変換行列
        const auto projection_matrix = camera.get_projection_matrix();
        shader.set_uniform("projectionMatrix", projection_matrix);

        // モデルの描画
        GLuint tex_id = material.texture.value_or(0);
        shader.set_uniform("is_tex", (tex_id == 0 ? GL_FALSE : GL_TRUE));
        shader.set_uniform("baseColor", material.base_color);
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

    void draw_observation(KeyType key, const Camera &camera) {
        auto &obs = observations[key];
        StaticMesh &mesh = *std::get<0>(key);
        const Material &material = *std::get<1>(key);
        const auto &model_matrices = obs.model_matrices; // モデル行列キュー

        // モデル行列vectorのメモリ再確保、もしくはメッシュのVBO更新等で再生成が必要ならば空のVBO&VAOを新規作成
        // TODO: メッシュのVBO更新の際はVAOの再生成だけで十分。model_matrices_vboの再生成は必要ない
        if (obs.matrices_raw != model_matrices.data() || mesh.vao_should_regen_) {
            obs.matrices_raw = model_matrices.data();
            auto model_matrices_vbo = VertexBufferObject(sizeof(glm::mat4) * model_matrices.capacity(), nullptr, GL_DYNAMIC_DRAW);
            auto batch_vao = generate_vao(mesh, material.shader, model_matrices_vbo);
            obs.vbovao = std::make_pair(std::move(model_matrices_vbo), std::move(batch_vao));
        }

        VertexBufferObject &model_matrices_vbo = obs.vbovao.first;
        const VertexArrayObject &batch_vao = obs.vbovao.second;

        // VBOに毎フレーム値をコピー
        model_matrices_vbo.subdata(0, sizeof(glm::mat4) * model_matrices.size(), model_matrices.data());

        // 描画
        draw_instanced(mesh, material, batch_vao, model_matrices.size(), camera);
    }

    void draw_all_registered_objects(const Camera &camera) {
        // マテリアルの優先度に基づいてキーをソート
        std::vector<KeyType> sorted_keys{};
        sorted_keys.reserve(observations.size());
        for (const auto &[key, obs] : observations) {
            sorted_keys.push_back(key);
        }
        std::sort(sorted_keys.begin(), sorted_keys.end(), [](const auto &key1, const auto &key2) {
            const Material &material1 = *std::get<1>(key1);
            const Material &material2 = *std::get<1>(key2);
            return material1.priority < material2.priority;
        });

        // モデル行列キューの一覧を走査
        for (const auto &key : sorted_keys) {
            draw_observation(key, camera);
        }
    }
};
