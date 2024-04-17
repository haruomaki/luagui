#pragma once

#include <WorldObject.hpp>

class Rigidbody : virtual public WorldObject {
  public:
    std::optional<std::function<void()>> callback;

    Rigidbody();
    ~Rigidbody() override;
};

class AABB2d : public Rigidbody {
  public:
    glm::vec2 center{0, 0};
    float width = 0;
    float height = 0;

    // AABB2d() {
    // }
};
