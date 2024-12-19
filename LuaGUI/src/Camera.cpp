#include "Camera.hpp"

void register_camera(sol::state &lua) {
    // Cameraクラス
    lua.new_usertype<Camera>("Camera", sol::base_classes, sol::bases<Component>());

    lua["WorldObject"]["add_camera_component"] = [&](WorldObject &obj, const std::string &projection_mode) -> Camera & {
        return obj.add_component<Camera>(projection_mode == "Orthogonal" ? Camera::Orthogonal : Camera::Normal);
    };
}
