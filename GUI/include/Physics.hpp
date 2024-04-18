#pragma once

#include <WorldObject.hpp>

class AABB2d;
class Circle;

class Rigidbody : virtual public WorldObject {
  public:
    std::optional<std::function<void()>> callback;

    Rigidbody();
    ~Rigidbody() override;

    virtual void collide(Rigidbody &) = 0;
    virtual void collide_aabb2d(AABB2d &) = 0;
    virtual void collide_circle(Circle &) = 0;
};

class AABB2d : public Rigidbody {
  public:
    glm::vec2 center{0, 0};
    float width = 0;
    float height = 0;

    // AABB2d() {
    // }
    void collide(Rigidbody &rb) override {
        rb.collide_aabb2d(*this);
    }
    void collide_aabb2d(AABB2d & /*rb*/) override {
        info("AABB2Dのcollide_aabb2dです");
    }
    void collide_circle(Circle & /*rb*/) override {
        info("AABB2Dのcollide_circleです");
    }
};

class Circle : public Rigidbody {
  public:
    glm::vec2 center{0, 0};
    float radius = 0;

    // AABB2d() {
    // }
    void collide(Rigidbody &rb) override {
        rb.collide_circle(*this);
    }
    void collide_aabb2d(AABB2d & /*rb*/) override {
        info("Circleのcollide_aabb2dです");
    }
    void collide_circle(Circle & /*rb*/) override {
        info("Circleのcollide_circleです");
    }
};
