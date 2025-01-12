#pragma once

#include "Camera.hpp"
#include "Material.hpp"
#include "ResourceUpdate.hpp"
#include "core.hpp"
#include <map>

// VRAMとの同期を毎フレーム自動で行わないメッシュ
class StaticMesh : virtual public Resource {
    friend class MeshComponent;
    friend struct MeshDrawManager;

  protected:
    bool vao_should_regen_ = true;
    GL::VertexBuffer vbo_;
    GL::ElementBuffer ebo_;
    const GLenum usage_;
    size_t n_ = 0;
    size_t indices_n_ = 0;
    size_t capacity_ = 0;
    size_t indices_capacity_ = 0;

    void regenerate_vbo() {
        info("VBO生成");
        // 空のVBOを生成
        this->vbo_ = GL::VertexBuffer(sizeof(InterleavedVertexInfo) * vertices.capacity(), nullptr, usage_);
        vao_should_regen_ = true;
    }

    void regenerate_ibo() {
        info("IBO生成");
        this->ebo_ = GL::ElementBuffer(sizeof(int) * indices.size(), indices.data(), usage_);
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

class MeshComponent : public Component {
  public:
    StaticMesh &mesh;
    Material &material;

    MeshComponent(StaticMesh &mesh, Material *material = nullptr);
    ~MeshComponent() override;
};

struct ModelMatricesObservation {
    vector<glm::mat4> model_matrices;
    std::unordered_map<const MeshComponent *, size_t> object_index_map;
    std::vector<const MeshComponent *> initial_list;
    std::vector<const MeshComponent *> delete_list;

    // メッシュ・マテリアル・シェーダ・"モデル行列の生配列"の四項組ごとに一つVBO&VAOが決まる
    const glm::mat4 *matrices_raw = nullptr;
    std::pair<GL::VertexBuffer, GL::VertexArray> vbovao;
};

struct MeshDrawManager {
    // メッシュ・マテリアル・シェーダの三項組ごとに一つモデル行列のvectorが決まる。
    using KeyType = std::tuple<StaticMesh *, const Material *, const GL::ProgramObject *>;
    // この行列キューごとに一回ドローコールを行う
    std::map<KeyType, ModelMatricesObservation> observations;

    static KeyType key_from(const MeshComponent *obj);
    void set_model_matrix(const MeshComponent *mc);
    void delete_model_matrix(const MeshComponent *obj);
    void step();
    static GL::VertexArray generate_vao(StaticMesh &mesh, const GL::ProgramObject &shader, const GL::VertexBuffer &model_matrices_vbo);
    static inline void draw_instanced(const StaticMesh &mesh, const Material &material, const GL::VertexArray &vao, size_t count_instances, const CameraInterface &camera);
    void draw_observation(KeyType key, const CameraInterface &camera);
    void draw_all_registered_objects(const CameraInterface &camera);
};
