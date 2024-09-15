#include "Box2D.hpp"

// b2::World::SetGravityなど、「b2Vec2を受け取るメンバ関数」を「テーブルを受け取るメンバ関数」に変換する高階関数。
static std::function<void(b2::World *, const sol::table &)> wrap_table(void (b2::World::*f)(b2Vec2)) {
    return [f](b2::World *self, const sol::table &tbl) {
        b2Vec2 vec = {tbl.get<float>(1), tbl.get<float>(2)};
        (self->*f)(vec);
    };
}

static void add_shape(b2::Body *body, const sol::table & /*tbl*/) {
    // TODO: tblにはShapeを作るときのオプション（形状の種類とそれに必要なパラメータ）を指定できる

    b2::Shape::Params shape_params;
    shape_params.friction = 100.f;

    body->CreateShape(
        b2::DestroyWithParent,
        shape_params,
        b2Circle{.center = b2Vec2(), .radius = 10});
}

void register_box2d(sol::state &lua) {
    // Rigidbodyコンポーネント
    lua.new_usertype<RigidbodyComponent>(
        "RigidbodyComponent",
        "body", sol::readonly_property([](RigidbodyComponent *rbc) { return &rbc->b2body; }));

    // Box2Dの各型
    lua.new_usertype<b2::World>(
        "b2World",
        "gravity", sol::property(&b2::World::GetGravity, wrap_table(&b2::World::SetGravity)));

    lua.new_usertype<b2::Body>(
        "b2Body",
        "add_shape", add_shape);

    lua.new_usertype<b2Vec2>(
        "b2Vec2",
        // sol::constructors<b2Vec2(), b2Vec2(float, float)>(),
        // "x", &b2Vec2::x,
        // "y", &b2Vec2::y,
        "__tostring", [](b2Vec2 &self) { return "b2Vec2(" + std::to_string(self.x) + ", " + std::to_string(self.y) + ")"; });
}
