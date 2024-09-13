#include <utility.hpp>

static void draw_line(World &world, std::vector<std::vector<float>> points) { // NOLINT(performance-unnecessary-value-param)
    std::vector<glm::vec3> coords(points.size());
    for (size_t i = 0; i < points.size(); i++) {
        coords[i].x = points[i][0];
        coords[i].y = points[i][1];
    }
    auto &line_obj = new_line(world);
    line_obj.mesh.vertices.setCoords(coords);
}

static void draw_circle(World &world, std::vector<float> center, float radius, std::optional<int> segments_opt) { // NOLINT(performance-unnecessary-value-param)
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
}

void register_world(sol::state &lua) {
    lua.set_function("create_world", [&]() -> World & {
        Window &window = lua["window"];

        auto &world = window.create_world();

        auto &camera = world.append_child<MobileOrthoCamera>();
        camera.set_active();

        return world;
    });

    lua.new_usertype<World>("World", "draw_line", draw_line, "draw_circle", draw_circle);
}
