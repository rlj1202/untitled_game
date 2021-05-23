#include "camera.h"

#include <glm/gtc/matrix_transform.hpp>

#define DEBUG
#include "debug.h"

Camera::Camera() : projection_mat(1), position(0), scale(1.0f) {
    Calculate();
}

Camera::Camera(glm::mat4 projection)
    : projection_mat(projection), position(0), scale(1.0f) {
    Calculate();
}

Camera::Camera(glm::mat4 projection, glm::vec3 position, float scale)
    : projection_mat(projection), position(position), scale(scale) {
    Calculate();
}

const glm::mat4& Camera::GetMatrix() const {
    return result_mat;
}

void Camera::SetProjection(const glm::mat4& mat) {
    projection_mat = mat;
    Calculate();
}

void Camera::SetPosition(const glm::vec3& position) {
    this->position = position;
    Calculate();
}

void Camera::SetScale(const float scale) {
    this->scale = scale;
    Calculate();
}

const glm::mat4& Camera::GetProjection() const {
    return projection_mat;
}

const glm::vec3& Camera::GetPosition() const {
    return position;
}

float Camera::GetScale() const {
    return scale;
}

glm::vec3 Camera::GetWorldCoords(const glm::vec3& homogeneous_coord) {
    // result_mat_inv = translate^-1 * scale^1 * projection_mat^-1
    result_mat_inv = glm::translate(glm::mat4(1), position);
    result_mat_inv = glm::scale(result_mat_inv, glm::vec3(1.0f / scale, 1.0f / scale, 0));
    result_mat_inv = result_mat_inv * glm::inverse(projection_mat);

    return result_mat_inv * glm::vec4(homogeneous_coord, 1.0f);
}

void Camera::Calculate() {
    // result_mat = projection_mat * scale * translate
    // A = projection_mat * scale * translate * B
    // translate^-1 * scale^-1 * projection_mat^-1 * A = B
    result_mat = glm::scale(projection_mat, glm::vec3(scale, scale, 0));
    result_mat = glm::translate(result_mat, -position);
}