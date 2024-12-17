#include "Camera.hpp"
#include "World.hpp"
#include <SumiGL/Context.hpp>

void Camera::set_active() {
    this->get_world().active_camera() = this;
}

glm::mat4 NormalCamera::get_view_matrix() const {
    return SCALE({-1, 1, -1}) * glm::inverse(get_absolute_transform());
}

glm::mat4 NormalCamera::get_projection_matrix() const {
    auto vp = this->get_world().viewport_provider();
    auto width = float(vp.width);
    auto height = float(vp.height);

    const auto aspect_ratio = width / height;
    const auto r = this->scale_prop;
    const auto projection_matrix = glm::perspective(glm::radians(60.0F), aspect_ratio, 0.01F * r, 1000.F * r);
    return projection_matrix;
}

glm::mat4 OrthoCamera::get_view_matrix() const {
    return glm::inverse(get_absolute_transform());
}

glm::mat4 OrthoCamera::get_projection_matrix() const {
    auto vp = this->get_world().viewport_provider();
    int width = vp.width;
    int height = vp.height;

    auto ms = this->get_world().window.gui.master_scale();
    const auto w = float(width) * ms.x;
    const auto h = float(height) * ms.y;
    const auto r = this->scale_prop;
    const auto projection_matrix = glm::ortho<float>(-w / 2, w / 2, -h / 2, h / 2, -1000.F * r, 1000.F * r);
    return projection_matrix;
}
