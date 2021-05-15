#ifndef H_CAMERA
#define H_CAMERA

#include <glm/glm.hpp>

class Camera {
public:
    Camera();

    glm::mat4 GetMatrix() const;

    void SetProjection(glm::mat4 mat);
    void SetPosition(glm::vec3 position);
    void SetScale(float scale);

private:
    glm::mat4 projection_mat;
    glm::vec3 position;
    // unit: pixels/1
    float scale;

    glm::mat4 result_mat;

    void Calculate();
};

#endif