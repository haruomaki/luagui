#pragma once

#include <WorldObject.hpp>

class AABB2d;
class Circle;

struct AbsAABB2d {
    float x1, y1, x2, y2;
};

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

    static AbsAABB2d to_abs(const AABB2d &self, const glm::vec2 &pos, const float scale) {
        auto center = self.center + pos;
        auto mx = self.width * scale / 2;
        auto my = self.height * scale / 2;
        return AbsAABB2d{center.x - mx, center.y - my, center.x + mx, center.y + my};
    }

    void collide(Rigidbody &rb) override {
        rb.collide_aabb2d(*this);
    }
    void collide_aabb2d(AABB2d &rb) override {
        print("AABB2Dのcollide_aabb2dです");
        auto obj1 = to_abs(*this, {this->get_absolute_position()}, this->get_absolute_scale_prop());
        auto obj2 = to_abs(rb, {rb.get_absolute_position()}, rb.get_absolute_scale_prop());

        bool judge = obj1.x1 < obj2.x2 && obj1.x2 > obj2.x1 &&
                     obj1.y1 < obj2.y2 && obj1.y2 > obj2.y1;
        if (judge) {
            print("ぶつかってるよ！");
        }
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
