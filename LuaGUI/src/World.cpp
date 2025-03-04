#include "World.hpp"
#include <GUI/utility.hpp>
#include <GUI/utility2.hpp>

static MeshComponent &draw_line(WorldObject &parent, std::vector<std::vector<float>> points, std::optional<bool> is_loop) { // NOLINT(performance-unnecessary-value-param)
    std::vector<glm::vec3> coords(points.size());
    for (size_t i = 0; i < points.size(); i++) {
        coords[i].x = points[i][0];
        coords[i].y = points[i][1];
    }
    auto &line_obj = new_line(parent);
    line_obj.mesh.vertices.setCoords(coords);
    if (is_loop.value_or(false)) {
        line_obj.mesh.draw_mode = GL_LINE_LOOP;
    }

    return line_obj;
}

static MeshComponent &draw_circle(WorldObject &parent, std::vector<float> center, float radius, std::optional<int> segments_opt) { // NOLINT(performance-unnecessary-value-param)
    const int segments = segments_opt.value_or(48);
    std::vector<glm::vec3> coords(segments + 1);

    for (int i = 0; i <= segments; ++i) {
        float theta = 2 * M_PIf * float(i) / float(segments); // 角度を計算
        float x = radius * cosf(theta);                       // x座標
        float y = radius * sinf(theta);                       // y座標
        coords[i] = glm::vec3(x, y, 0);
    }
    auto &line_obj = new_line(parent);
    line_obj.owner().position = {center[0], center[1], 0};
    line_obj.mesh.vertices.setCoords(coords);

    return line_obj;
}

static MeshComponent &draw_rect(WorldObject &parent, float hx, float hy) { // NOLINT(performance-unnecessary-value-param)
    std::vector<glm::vec3> coords = {{-hx, -hy, 0}, {hx, -hy, 0}, {hx, hy, 0}, {-hx, hy, 0}};
    auto &rect_obj = new_rect(parent, coords, MaterialBuilder().build(parent.get_world().gui));

    return rect_obj;
}

void register_world(sol::state &lua) {
    lua.set_function("create_world", [&](std::optional<bool> debug) -> World & {
        GUI &gui = lua["__GUI"];
        Window &window = lua["__CurrentWindow"];

        auto &world = gui.create_world();
        lua["__CurrentWorld"] = &world;

        if (debug) {
            mobile_ortho_camera(world.root, window);
        }

        // auto migmix_font = new Font();
        // world.append_child<Text>(*migmix_font, "This is sample text 123456789", RGBA{0.5, 0.8, 0.2, 0.4});

        return world;
    });

    lua.new_usertype<World>(
        "World",
        "b2world", sol::readonly_property([](World *world) { return &world->b2world; }),
        "bullet_world", sol::readonly_property([](World *world) { return &world->bullet_world; }),
        "raycast", [](World *w, glm::vec3 origin, glm::vec3 direction) { return w->raycast({origin.x, origin.y, origin.z}, {direction.x, direction.y, direction.z}); },
        "root", &World::root);

    // TODO: WorldObject.cppなどへ移動
    lua["WorldObject"]["draw_line"] = draw_line;
    lua["WorldObject"]["draw_circle"] = draw_circle;
    lua["WorldObject"]["draw_rect"] = draw_rect;
}
