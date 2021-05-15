#include "camera.h"

#include <glm/gtc/matrix_transform.hpp>

Camera::Camera() : projection_mat(1), position(0), scale(1.0f) {
    Calculate();
}

glm::mat4 Camera::GetMatrix() const {
    return result_mat;
}

void Camera::SetProjection(glm::mat4 mat) {
    projection_mat = mat;
    Calculate();
}

void Camera::SetPosition(glm::vec3 position) {
    this->position = position;
    Calculate();
}

void Camera::SetScale(float scale) {
    this->scale = scale;
    Calculate();
}

void Camera::Calculate() {
    result_mat = glm::scale(projection_mat, glm::vec3(scale, scale, 0));
    result_mat = glm::translate(result_mat, position);
}