#include "Box2D.hpp"

// b2::World::SetGravityなど、「b2Vec2を受け取るメンバ関数」を「テーブルを受け取るメンバ関数」に変換する高階関数。
static std::function<void(b2::World *, const sol::table &)> wrap_table(void (b2::World::*f)(b2Vec2)) {
    return [f](b2::World *self, const sol::table &tbl) {
        b2Vec2 vec = {tbl.get<float>(1), tbl.get<float>(2)};
        (self->*f)(vec);
    };
}

static void add_shape(RigidbodyComponent *rbc, const sol::table &tbl) {
    // tblにはShapeを作るときのオプション（形状の種類とそれに必要なパラメータ）を指定できる

    // shape（衝突形状）を取得
    std::string shape = tbl["shape"].get_or<const char *>("circle");

    if (shape == "circle") {
        std::vector<float> center = tbl["center"].get_or(std::vector<float>{0, 0});
        auto x = center.at(0);
        auto y = center.at(1);

        float radius = tbl["radius"].get<float>();

        b2::Shape::Params shape_params;
        shape_params.friction = 0.1f;

        rbc->b2body.CreateShape(
            b2::DestroyWithParent,
            shape_params,
            b2Circle{.center = b2Vec2{x, y}, .radius = radius});
    } else if (shape == "edge") {
        using Points = std::vector<std::vector<float>>;
        Points points = tbl["points"].get_or(Points{});
        auto x1 = points.at(0).at(0);
        auto y1 = points.at(0).at(1);
        auto x2 = points.at(1).at(0);
        auto y2 = points.at(1).at(1);

        b2::Shape::Params shape_params;
        shape_params.friction = 0.1f;

        rbc->b2body.CreateShape(
            b2::DestroyWithParent,
            shape_params,
            b2Segment{{x1, y1}, {x2, y2}});
    } else {
        warn("未知の形状種です: ", shape);
    }
}

void register_box2d(sol::state &lua) {
    // Rigidbodyコンポーネント
    lua.new_usertype<RigidbodyComponent>(
        "Rigidbody",
        "add_shape", add_shape);

    // Box2Dの各型
    lua.new_usertype<b2::World>(
        "b2World",
        "gravity", sol::property(&b2::World::GetGravity, wrap_table(&b2::World::SetGravity)));

    lua.new_usertype<b2Vec2>(
        "b2Vec2",
        // sol::constructors<b2Vec2(), b2Vec2(float, float)>(),
        // "x", &b2Vec2::x,
        // "y", &b2Vec2::y,
        "__tostring", [](b2Vec2 &self) { return "b2Vec2(" + std::to_string(self.x) + ", " + std::to_string(self.y) + ")"; });
}
