#include "Physics.hpp"
#include "World.hpp"

Rigidbody::Rigidbody() {
    this->get_world().rigidbodies.request_set(this);
}

Rigidbody::~Rigidbody() {
    this->get_world().rigidbodies.request_erase(this);
}
