#include "Mesh.hpp"
#include "World.hpp"

void MeshObject::draw(const Camera & /*camera*/) const {
    this->get_world().mesh_draw_manager_.register_to_draw(*this);
}
