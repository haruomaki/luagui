#pragma once

#include <Window.hpp>

class Camera : public WorldObject {
  public:
    // glm::mat4 view_matrix_ = glm::mat4(1);
    glm::mat4 projection_matrix_ = glm::perspective(glm::radians(90.0F), 1.f, 0.1F, 1000.f);

    [[nodiscard]] glm::mat4 getViewMatrix() const {
        return glm::inverse(getAbsoluteTransform());
    }
};
