#include "Mesh.hpp"
#include "World.hpp"

MeshComponent::MeshComponent(StaticMesh &mesh, Material *material)
    : mesh(mesh)
    , material(material == nullptr ? *this->world().gui.default_material : *material) {
    // 描画のための位置初期設定
    this->owner().position = this->owner().get_position();
}

MeshComponent::~MeshComponent() {
    // 描画の登録解除
    this->world().mesh_draw_manager_.delete_model_matrix(this);
}
