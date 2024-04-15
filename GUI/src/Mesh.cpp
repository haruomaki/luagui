#include "Mesh.hpp"
#include "World.hpp"

MeshObject::MeshObject(StaticMesh &mesh, Material *material)
    : mesh(mesh)
    , material(material == nullptr ? *this->get_world().window.default_material : *material) {}

MeshObject::~MeshObject() {
    // 描画の登録解除
    this->get_world().mesh_draw_manager_.delete_model_matrix(this);
}
