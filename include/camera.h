#ifndef H_CAMERA
#define H_CAMERA

#include <glm/glm.hpp>

class Camera {
public:
    Camera();
    Camera(glm::mat4 projection);
    Camera(glm::mat4 projection, glm::vec3 position, float scale);

    const glm::mat4& GetMatrix() const;

    void SetProjection(const glm::mat4& mat);
    void SetPosition(const glm::vec3& position);
    void SetScale(const float scale);

    const glm::mat4& GetProjection() const;
    const glm::vec3& GetPosition() const;
    float GetScale() const;

private:
    glm::mat4 projection_mat;
    glm::vec3 position;
    // unit: pixels/1
    float scale;

    glm::mat4 result_mat;

    void Calculate();
};

#endif