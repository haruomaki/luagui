#include "Camera.hpp"
#include <GUI/World.hpp>

using CenteringMap = std::unordered_map<std::string, Camera::Centering>;

static const CenteringMap &centering_map() {
    auto *p = new CenteringMap{
        {"Center", Camera::Center},
        {"TopLeft", Camera::TopLeft},
        {"BottomLeft", Camera::BottomLeft},
        {"TopRight", Camera::TopRight},
        {"BottomRight", Camera::BottomRight},
    };
    return *p;
}
static Camera::Centering from_string(const std::string &name) {
    return centering_map().at(name);
}
static std::string to_string(Camera::Centering centering) {
    for (const auto &[s, c] : centering_map()) {
        if (c == centering) return s;
    }
    throw std::runtime_error("未知のバリアントです。");
}

void register_camera(sol::state &lua) {
    // Cameraクラス
    lua.new_usertype<Camera>(
        "Camera",
        "centering", sol::property([](Camera *c) { return to_string(c->centering); }, [](Camera *c, const std::string &name) { c->centering = from_string(name); }),
        "projection_mode", sol::readonly_property([](Camera *c) { return c->projection_mode == Camera::Perspective ? "Perspective" : "Orthographic"; }),
        "active", &Camera::active,
        "toggle_mode", &Camera::toggle_mode,
        sol::base_classes, sol::bases<Component>());

    lua["WorldObject"]["add_camera_component"] = [&](WorldObject &obj, const std::string &projection_mode) -> Camera & {
        Window *window = lua["__CurrentWindow"];
        return obj.add_component<Camera>(window, projection_mode == "Orthographic" ? Camera::Orthographic : Camera::Perspective);
    };
}
