#include "Mesh.hpp"
#include "World.hpp"
#include "WorldObject.hpp"

MeshComponent::MeshComponent(StaticMesh &mesh, Material *material)
    : mesh(mesh)
    , material(material == nullptr ? *this->world().gui.resources.find<Material>("default_material") : *material) {
    // 描画のための位置初期設定
    this->owner().position = this->owner().get_position();
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

    if (observations.contains(key)) {
        auto &obj_ix_map = observations[key].object_index_map;

        if (obj_ix_map.contains(mc)) {
            // すでに登録済みのメッシュオブジェクトの場合、ただちに書き換え
            size_t index = obj_ix_map[mc];
            const auto &model_matrix = mc->owner().get_absolute_transform();
            observations[key].model_matrices[index] = model_matrix;
        } else {
            // 新たなメッシュオブジェクトの場合、一旦initial_valuesに蓄えておく
            observations[key].initial_list.emplace_back(mc);
        }
    } else {
        // そもそも初めてのkeyのオブジェクトだった場合、キーを追加してやはりinitial_valuesに蓄える
        observations[key] = ModelMatricesObservation{};
        observations[key].initial_list.emplace_back(mc);
    }
}

void MeshDrawManager::delete_model_matrix(const MeshComponent *obj) {
    auto key = key_from(obj);
    assert(observations.contains(key)); // 一度も登録されていないキーを持つオブジェクトの削除要求
    observations[key].delete_list.push_back(obj);
}

void MeshDrawManager::step() {
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
            for (auto &[mc, index] : obj_ix_map) {
                const auto &model_matrix = mc->owner().get_absolute_transform();
                index = counter++;
                obs.model_matrices[index] = model_matrix;
            }
        }

        // 各キューを削除
        obs.initial_list = std::vector<const MeshComponent *>();
        obs.delete_list = std::vector<const MeshComponent *>();
    }

    // 使われなくなったモデル行列キューは削除する
    std::erase_if(observations, [](const auto &item) {
        const auto &[key, obs] = item;
        return (obs.model_matrices.size() == 0); // 必ず obs.object_index_map == 0 でもあるはず
    });
}

GL::VertexArray MeshDrawManager::generate_vao(StaticMesh &mesh, const GL::ProgramObject &shader, const GL::VertexBuffer &model_matrices_vbo) {
    info("VAO生成");
    auto vao = GL::VertexArray();

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

void MeshDrawManager::draw_instanced(const StaticMesh &mesh, const Material &material, const GL::VertexArray &vao, size_t count_instances, const CameraInterface &camera) {
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

    // モデルの描画
    GLuint tex_id = material.texture.get();
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

void MeshDrawManager::draw_observation(KeyType key, const CameraInterface &camera) {
    auto &obs = observations[key];
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
    draw_instanced(mesh, material, batch_vao, model_matrices.size(), camera);
}

void MeshDrawManager::draw_all_registered_objects(const CameraInterface &camera) {
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
