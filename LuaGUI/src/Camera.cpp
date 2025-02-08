#include "Camera.hpp"
#include <GUI/World.hpp>

void register_camera(sol::state &lua) {
    // Cameraクラス
    lua.new_usertype<Camera>(
        "Camera",
        "centering", &Camera::centering,
        "projection_mode", sol::readonly_property([](Camera *c) { return c->projection_mode == Camera::Perspective ? "Perspective" : "Orthographic"; }),
        "active", &Camera::active,
        "toggle_mode", &Camera::toggle_mode,
        sol::base_classes, sol::bases<Component>());

    lua["WorldObject"]["add_camera_component"] = [&](WorldObject &obj, const std::string &projection_mode) -> Camera & {
        Window *window = lua["__CurrentWindow"];
        return obj.add_component<Camera>(window, projection_mode == "Orthographic" ? Camera::Orthographic : Camera::Perspective);
    };
}
