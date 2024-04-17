#include "Physics.hpp"
#include "World.hpp"

Rigidbody::Rigidbody() {
    this->get_world().rigidbodies.insert(this);
}

Rigidbody::~Rigidbody() {
    this->get_world().rigidbodies.erase(this);
}
