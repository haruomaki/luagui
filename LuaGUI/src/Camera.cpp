#include "Camera.hpp"
#include <GUI/World.hpp>

void register_camera(sol::state &lua) {
    // Cameraクラス
    lua.new_usertype<Camera>(
        "Camera",
        "active", &Camera::active,
        sol::base_classes, sol::bases<Component>());

    lua["WorldObject"]["add_camera_component"] = [&](WorldObject &obj, const std::string &projection_mode) -> Camera & {
        // Window &window = lua["__CurrentWindow"];
        // FIXME: __CurrentWindowにはHandleを入れる？
        ResourceHandle<Window> window = lua["__CurrentWindowResource"];
        return obj.add_component<Camera>(window, projection_mode == "Orthographic" ? Camera::Orthographic : Camera::Perspective);
    };
}
