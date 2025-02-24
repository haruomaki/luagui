#pragma once

#include "Camera.hpp"
#include "Material.hpp"
#include "ResourceUpdate.hpp"
#include "core.hpp"
#include <map>

// VRAMとの同期を毎フレーム自動で行わないメッシュ
class StaticMesh : virtual public Resource {
    friend class MeshComponent;
    friend class MeshDrawManager;

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

    StaticMesh(GLenum draw_mode = GL_TRIANGLE_STRIP, const std::vector<glm::vec3> &coords = {}, const std::vector<RGBA> &colors = {}, const std::vector<glm::vec2> &uvs = {}, GLenum usage = GL_STATIC_DRAW)
        : usage_(usage)
        , n_(coords.size())
        , capacity_(coords.capacity())
        , draw_mode(draw_mode) {
        std::vector<InterleavedVertexInfo> vers(n_);
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
    Mesh(GLenum draw_mode = GL_TRIANGLE_STRIP, const std::vector<glm::vec3> &coords = {}, const std::vector<RGBA> &colors = {}, const std::vector<glm::vec2> &uvs = {})
        : StaticMesh(draw_mode, coords, colors, uvs, GL_DYNAMIC_DRAW) {}
};

class MeshComponent : public Component {
  public:
    StaticMesh &mesh;
    Material &material;

    MeshComponent(StaticMesh &mesh, Material *material = nullptr);
    ~MeshComponent() override;
};

// モデル行列の保持、および追加/削除を担う構造体。
struct ModelMatricesObservation {
    // 同一のKey三項組を持つ物体たちのモデル行列をひとまとめに格納する。
    // 物体が一つでも追加/削除されるたび、再生成される。
    std::vector<glm::mat4> model_matrices;
    // とある物体のモデル行列がmodel_matrices内のどの位置に格納されているかを記録する。
    // ここのキーに無い物体は、そもそもモデル行列が格納されていない。
    // 物体が一つでも追加/削除されるたび、model_matricesに付随して再生成される。
    BufferedMap<const MeshComponent *, size_t, false> object_index_map;

    // 普段はmodel_matricesのdata()と一致する。vector内部でメモリ再確保が行われるのを検知するのに利用。
    const glm::mat4 *matrices_raw = nullptr;
    // メッシュ・マテリアル・シェーダ・"モデル行列の生配列"の四項組ごとに一つVBO&VAOが決まる
    std::pair<GL::VertexBuffer, GL::VertexArray> vbovao;
};

class MeshDrawManager {
    // メッシュ・マテリアル・シェーダの三項組ごとに一つモデル行列のvectorが決まる。
    using KeyType = std::tuple<StaticMesh *, const Material *, const GL::ProgramObject *>;
    // この行列キューごとに一回ドローコールを行う
    std::map<KeyType, ModelMatricesObservation> observations_;

    static KeyType key_from(const MeshComponent *obj);
    static GL::VertexArray generate_vao(StaticMesh &mesh, const GL::ProgramObject &shader, const GL::VertexBuffer &model_matrices_vbo);
    // マテリアルに基づいて描画設定しつつ、インスタンス描画を実行する。
    static inline void drawcall(const StaticMesh &mesh, const Material &material, const GL::VertexArray &vao, size_t count_instances, const CameraInterface &camera);
    // 一つのKey三項組に対して一回の描画処理（VAOなどの更新＆ドローコール）を行う。
    void draw_observation(KeyType key, const CameraInterface &camera);

    friend class World; // stepとdraw_allため
    void step();
    void draw_all(const CameraInterface &camera);

  public:
    void set_model_matrix(const MeshComponent *mc);
    void delete_model_matrix(const MeshComponent *obj);
};
