#include <utility.hpp>

static MeshObject &draw_line(World &world, std::vector<std::vector<float>> points) { // NOLINT(performance-unnecessary-value-param)
    std::vector<glm::vec3> coords(points.size());
    for (size_t i = 0; i < points.size(); i++) {
        coords[i].x = points[i][0];
        coords[i].y = points[i][1];
    }
    auto &line_obj = new_line(world);
    line_obj.mesh.vertices.setCoords(coords);

    line_obj.add_component<RigidbodyComponent>();

    return line_obj;
}

static MeshObject &draw_circle(World &world, std::vector<float> center, float radius, std::optional<int> segments_opt) { // NOLINT(performance-unnecessary-value-param)
    const int segments = segments_opt.value_or(48);
    std::vector<glm::vec3> coords(segments + 1);

    for (int i = 0; i <= segments; ++i) {
        float theta = 2 * M_PIf * float(i) / float(segments); // 角度を計算
        float x = radius * cosf(theta);                       // x座標
        float y = radius * sinf(theta);                       // y座標
        coords[i] = glm::vec3(center[0] + x, center[1] + y, 0);
    }
    auto &line_obj = new_line(world);
    line_obj.mesh.vertices.setCoords(coords);

    return line_obj;
}

// b2::World::SetGravityなど、「b2Vec2を受け取るメンバ関数」を「テーブルを受け取るメンバ関数」に変換する高階関数。
static std::function<void(b2::World *, sol::table)> wrap_table(void (b2::World::*f)(b2Vec2)) {
    return [f](b2::World *self, sol::table tbl) {
        b2Vec2 vec = {tbl.get<float>(1), tbl.get<float>(2)};
        (self->*f)(vec);
    };
}

void register_world(sol::state &lua) {
    lua.set_function("create_world", [&]() -> World & {
        Window &window = lua["window"];

        auto &world = window.create_world();

        auto &camera = world.append_child<MobileOrthoCamera>();
        camera.set_active();

        return world;
    });

    lua.new_usertype<World>(
        "World",
        "draw_line", draw_line,
        "draw_circle", draw_circle,
        "b2world", sol::readonly_property([](World *world) { return &world->b2world; }));

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
