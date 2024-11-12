#pragma once

#include "World.hpp"

class AABB2d;
class Circle;

struct AbsAABB2d {
    float x1, y1, x2, y2;
};

class Rigidbody : virtual public WorldObject {
  public:
    std::optional<std::function<void(Rigidbody &self, Rigidbody &other)>> callback{};
    glm::vec3 velocity{0, 0, 0};
    bool moving = true;

    Rigidbody();
    ~Rigidbody() override;

    static void invoke_callbacks(Rigidbody &rb1, Rigidbody &rb2) {
        if (rb1.callback.has_value()) {
            rb1.callback.value()(rb1, rb2);
        }
        if (rb2.callback.has_value()) {
            rb2.callback.value()(rb2, rb1);
        }
    }

    virtual void tick() {
        // 速度や加速度を適用して時間を1tick進める
        int fps = this->get_world().window.refresh_rate();
        this->position += this->velocity / fps;
        this->velocity *= 0.99;
    }

    virtual void collide(Rigidbody &) = 0;
    virtual void collide_aabb2d(AABB2d &) = 0;
    virtual void collide_circle(Circle &) = 0;
};

class AABB2d : public Rigidbody {
    AbsAABB2d current_abs_{};
    AbsAABB2d previous_abs_{}; // 衝突方向の判定のために1フレーム前の位置を記憶しておく

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

    static inline bool test(float l1, float r1, float l2, float r2) {
        // ¬ (l1 < l2 || r2 < l1)
        return r1 > l2 && r2 > l1;
    }

    void tick() override {
        // static_cast<Rigidbody *>(this)->tick(); // 速度に基づいて移動などは引き続き行う
        // FIXME: スーパークラスの仮想関数を呼びたい
        int fps = this->get_world().window.refresh_rate();
        this->position += this->velocity / fps;
        this->velocity *= 0.99;

        this->previous_abs_ = this->current_abs_;
        this->current_abs_ = to_abs(*this, {this->get_absolute_position()}, this->get_absolute_scale_prop());
    }

    void collide(Rigidbody &rb) override {
        rb.collide_aabb2d(*this);
    }
    void collide_aabb2d(AABB2d &rb) override;
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
