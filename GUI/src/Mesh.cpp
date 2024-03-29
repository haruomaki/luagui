#include "Mesh.hpp"
#include "World.hpp"

MeshObject::MeshObject(StaticMesh &mesh, Material *material)
    : mesh(mesh)
    , material(material == nullptr ? this->get_world().window.default_material : material) {}

void MeshObject::draw(const Camera & /*camera*/) const {
    this->get_world().mesh_draw_manager_.register_to_draw(*this);
}
