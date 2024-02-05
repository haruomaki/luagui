#include "World.hpp"
#include <Camera.hpp>

Camera::~Camera() = default;

void Camera::set_active() {
    this->get_world()->set_active_camera(*this);
}

glm::mat4 NormalCamera::getViewMatrix() const {
    return SCALE({-1, 1, -1}) * glm::inverse(getAbsoluteTransform());
}

glm::mat4 NormalCamera::getProjectionMatrix() const {
    auto size = viewport_.get_size();
    auto width = float(size.x);
    auto height = float(size.y);

    const auto aspect_ratio = width / height;
    const auto projection_matrix = glm::perspective(glm::radians(60.0F), aspect_ratio, 0.1F, 3000.F);
    return projection_matrix;
}

glm::mat4 OrthoCamera::getViewMatrix() const {
    return glm::inverse(getAbsoluteTransform());
}

glm::mat4 OrthoCamera::getProjectionMatrix() const {
    auto size = viewport_.get_size();
    int width = size.x;
    int height = size.y;

    const auto w = float(width);
    const auto h = float(height);
    const auto projection_matrix = glm::ortho<float>(-w / 2, w / 2, -h / 2, h / 2, 1000.F, -1000.F);
    return projection_matrix;
}
