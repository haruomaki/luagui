#include "Mesh.hpp"
#include "World.hpp"
#include "WorldObject.hpp"
#include <algorithm>

MeshComponent::MeshComponent(StaticMesh &mesh, Material *material)
    : mesh(mesh)
    , material(material == nullptr ? *this->world().gui.resources.find<Material>("default_material") : *material) {
    // コンポーネント作成直後から描画されるように。
    world().mesh_draw_manager_.set_model_matrix(this);
}

MeshComponent::~MeshComponent() {
    // 描画の登録解除
    this->world().mesh_draw_manager_.delete_model_matrix(this);
}

// -----------------
// MeshDrawManager
// -----------------

MeshDrawManager::KeyType MeshDrawManager::key_from(const MeshComponent *obj) {
    StaticMesh *mesh = &obj->mesh;
    const Material *material = &obj->material;
    const GL::ProgramObject *shader = &obj->material.shader;
    auto key = std::make_tuple(mesh, material, shader);
    return key;
}

void MeshDrawManager::set_model_matrix(const MeshComponent *mc) {
    auto key = key_from(mc);

    if (observations_.contains(key)) {
        auto &obj_ix_map = observations_[key].object_index_map;

        if (obj_ix_map.contains(mc)) {
            // すでに登録済みのメッシュオブジェクトの場合、ただちに書き換え
            size_t index = obj_ix_map[mc];
            const auto &model_matrix = mc->owner().get_absolute_transform();
            observations_[key].model_matrices[index] = model_matrix;
        } else {
            // 新たなメッシュオブジェクトの場合、一旦リクエストに蓄えておく（-1はダミー値）
            observations_[key].object_index_map.request_set(mc, -1);
        }
    } else {
        // そもそも初めてのkeyのオブジェクトだった場合、キーを追加してやはりリクエストに蓄える
        observations_[key] = ModelMatricesObservation{};
        observations_[key].object_index_map.request_set(mc, -1);
    }
}

void MeshDrawManager::delete_model_matrix(const MeshComponent *obj) {
    auto key = key_from(obj);
    assert(observations_.contains(key)); // 一度も登録されていないキーを持つオブジェクトの削除要求
    observations_[key].object_index_map.request_erase(obj);
}

void MeshDrawManager::step() {
    // 追加/削除リクエストキューが空でないすべてのキーについて、モデル行列配列を再生成
    for (auto &[key, obs] : observations_) {
        if (obs.object_index_map.is_dirty()) {
            auto &obj_ix_map = obs.object_index_map;

            // 登録予定/削除予定を実際に反映。
            obj_ix_map.flush<false>();

            // この時点では、obj_ix_mapのキーのみに意味がある。
            // キーが物体の一覧を意味する。

            // インデックスの振り直しとモデル行列行列の再生成を一挙に行う
            auto arr_size = obj_ix_map.size();
            obs.model_matrices = std::vector<glm::mat4>(arr_size); // モデル行列配列再生成
            size_t counter = 0;
            for (auto &[mc, index] : obj_ix_map) {
                const auto &model_matrix = mc->owner().get_absolute_transform();
                index = counter++; // indexは参照であることに注意。
                obs.model_matrices[index] = model_matrix;
            }
        }
    }

    // 使われなくなったモデル行列行列は削除する
    std::erase_if(observations_, [](const std::pair<const KeyType, ModelMatricesObservation> &item) {
        const auto &[key, obs] = item;
        if (obs.model_matrices.size() == 0) {
            assert(!obs.object_index_map.is_dirty() && obs.object_index_map.size() == 0); // 物体一覧も必ず空であるはず
            return true;
        }
        return false;
    });
}

GL::VertexArray MeshDrawManager::generate_vao(StaticMesh &mesh, const GL::ProgramObject &shader, const GL::VertexBuffer &model_matrices_vbo) {
    info("VAO生成");
    auto vao = GL::VertexArray();
    // TODO: gen関数を作成

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

void MeshDrawManager::drawcall(const StaticMesh &mesh, const Material &material, const GL::VertexArray &vao, size_t count_instances, const CameraInterface &camera) {
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
        break;
    }

    // ワールド座標変換 = instanceModelMatrix

    // ビュー座標変換
    const glm::mat4 &view_matrix = camera.get_view_matrix();
    shader.set_uniform("viewMatrix", view_matrix);

    // 射影変換行列
    const auto projection_matrix = camera.get_projection_matrix();
    shader.set_uniform("projectionMatrix", projection_matrix);

    // TODO: 「深度テストを行うかどうか」の設定をマテリアルに追加する。

    // デプスバッファに書き込むかどうか
    glDepthMask(material.write_depth ? GL_TRUE : GL_FALSE);

    // ポリゴンの裏面を描画するかどうか
    material.both ? glDisable(GL_CULL_FACE) : glEnable(GL_CULL_FACE);

    // モデルの描画
    shader.set_uniform("is_tex", (material.texture.is_valid() ? GL_TRUE : GL_FALSE));
    shader.set_uniform("baseColor", material.base_color);
    vao.bind([&] {
        material.texture.bind([&] {
            if (mesh.use_index) {
                size_t indices_length = mesh.indices_n_;
                glDrawElementsInstanced(mesh.draw_mode, GLsizei(indices_length), GL_UNSIGNED_INT, nullptr, GLsizei(count_instances));
                // 参考：glDrawElementsInstancedの引数
                // 1. mode - 描画モード。GL_TRIANGLESなど
                // 2. count - インデックスの個数。
                // 3. type - インデックスのデータ型。（例: GL_UNSIGNED_BYTE, GL_UNSIGNED_SHORT, GL_UNSIGNED_INT）
                // 4. indices - インデックスデータの開始位置。nullptrを指定するとEBOから読み込む。
                // 5. primcount - インスタンスの個数。
                // 頂点座標はバインド中のVBOから勝手に読み込まれる。
            } else {
                size_t vertices_length = mesh.n_;
                glDrawArraysInstanced(mesh.draw_mode, 0, GLsizei(vertices_length), GLsizei(count_instances));
            }
        });
    });
}

void MeshDrawManager::draw_observation(KeyType key, const CameraInterface &camera) {
    auto &obs = observations_[key];
    StaticMesh &mesh = *std::get<0>(key);
    const Material &material = *std::get<1>(key);
    const auto &model_matrices = obs.model_matrices; // モデル行列キュー

    // モデル行列vectorのメモリ再確保、もしくはメッシュのVBO更新等で再生成が必要ならば空のVBO&VAOを新規作成
    // TODO: メッシュのVBO更新の際はVAOの再生成だけで十分。model_matrices_vboの再生成は必要ない
    if (obs.matrices_raw != model_matrices.data() || mesh.vao_should_regen_) {
        obs.matrices_raw = model_matrices.data();
        auto model_matrices_vbo = GL::VertexBuffer(sizeof(glm::mat4) * model_matrices.capacity(), nullptr, GL_DYNAMIC_DRAW);
        auto batch_vao = generate_vao(mesh, material.shader, model_matrices_vbo);
        obs.vbovao = std::make_pair(std::move(model_matrices_vbo), std::move(batch_vao));
    }

    GL::VertexBuffer &model_matrices_vbo = obs.vbovao.first;
    const GL::VertexArray &batch_vao = obs.vbovao.second;

    // VBOに毎フレーム値をコピー
    model_matrices_vbo.subdata(0, sizeof(glm::mat4) * model_matrices.size(), model_matrices.data());

    // 描画
    drawcall(mesh, material, batch_vao, model_matrices.size(), camera);
}

void MeshDrawManager::draw_all(const CameraInterface &camera) {
    // マテリアルの優先度に基づいてキーをソート
    std::vector<KeyType> sorted_keys{};
    sorted_keys.reserve(observations_.size());
    for (const auto &[key, obs] : observations_) {
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
