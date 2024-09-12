#include <utility.hpp>

void register_world(sol::state &lua) {
    lua.set_function("create_world", [&]() -> World & {
        Window &window = lua["window"];

        auto &world = window.create_world();

        auto &camera = world.append_child<MobileOrthoCamera>();
        camera.set_active();

        return world;
    });

    lua.new_usertype<World>("World",
                            "draw_line", [](World &world, std::vector<std::vector<float>> points) { // NOLINT(performance-unnecessary-value-param)
                                std::vector<glm::vec3> coords(points.size());
                                for (size_t i = 0; i < points.size(); i++) {
                                    coords[i].x = points[i][0];
                                    coords[i].y = points[i][1];
                                }
                                auto &line_obj = new_line(world);
                                // line_obj.mesh.vertices.setCoords({{0, 0, 0}, {0.05, 0, 0}});
                                line_obj.mesh.vertices.setCoords(coords);
                            });
}
