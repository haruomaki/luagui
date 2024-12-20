Camera::Camera(ProjectionMode projection_mode)
    : projection_mode(projection_mode) {}

glm::mat4 Camera::get_view_matrix() const {
    switch (projection_mode) {
    case Perspective:
        return SCALE({-1, 1, -1}) * glm::inverse(owner().get_absolute_transform());
    case Orthographic:
        return glm::inverse(owner().get_absolute_transform());
    }
}

glm::mat4 Camera::get_projection_matrix() const {
    auto vp = this->world().viewport_provider();
    auto width = float(vp.width);
    auto height = float(vp.height);

    if (projection_mode == Perspective) {
        const auto aspect_ratio = width / height;
        const auto r = owner().scale_prop;
        const auto projection_matrix = glm::perspective(glm::radians(60.0F), aspect_ratio, 0.01F * r, 1000.F * r);
        return projection_matrix;
    }

    auto ms = this->world().window.gui.master_scale();
    const auto w = float(width) * ms.x;
    const auto h = float(height) * ms.y;
    const auto r = owner().get_scale().z;
    const auto near = -1000.0f * r;
    const auto far = 1000.0f * r;
    switch (mode) {
    case CameraMode::Center:
        return glm::ortho(-w / 2, w / 2, -h / 2, h / 2, near, far);
    case CameraMode::TopRight:
        return glm::ortho(-w, 0.0f, -h, 0.0f, near, far);
    case CameraMode::BottomRight:
        return glm::ortho(-w, 0.0f, 0.0f, h, near, far);
    case CameraMode::TopLeft:
        return glm::ortho(0.0f, w, -h, 0.0f, near, far);
    case CameraMode::BottomLeft:
        return glm::ortho(0.0f, w, 0.0f, h, near, far);
        // case CameraMode::Custom:
        //     // カスタム設定に基づく計算
        //     return glm::ortho(
        //         custom_origin.x - w / 2 * custom_scale,
        //         custom_origin.x + w / 2 * custom_scale,
        //         custom_origin.y - h / 2 * custom_scale,
        //         custom_origin.y + h / 2 * custom_scale,
        //         custom_origin.z - 1000.0f,
        //         custom_origin.z + 1000.0f);
    }
    throw std::runtime_error("Unsupported camera mode");
}
