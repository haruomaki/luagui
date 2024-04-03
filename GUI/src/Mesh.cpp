#include "Mesh.hpp"
#include "World.hpp"

MeshObject::MeshObject(StaticMesh &mesh, Material *material)
    : mesh(mesh)
    , material(material == nullptr ? *this->get_world().window.default_material : *material) {}
