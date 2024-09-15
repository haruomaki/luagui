#include "Box2D.hpp"

// b2::World::SetGravityなど、「b2Vec2を受け取るメンバ関数」を「テーブルを受け取るメンバ関数」に変換する高階関数。
static std::function<void(b2::World *, const sol::table &)> wrap_table(void (b2::World::*f)(b2Vec2)) {
    return [f](b2::World *self, const sol::table &tbl) {
        b2Vec2 vec = {tbl.get<float>(1), tbl.get<float>(2)};
        (self->*f)(vec);
    };
}

void register_box2d(sol::state &lua) {
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
