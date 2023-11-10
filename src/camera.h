#include "common.h"

#include "glm/glm.hpp"

struct Camera {
	glm::mat4 projection;
    glm::mat4 view;
	glm::vec3 position;

    f32 fov;
    f32 aspect;
    f32 near = 0.0f;
    f32 far = 0.0f;

    f64 yaw;
    f64 pitch;
};

Camera *CreateCamera(f32 fov, f32 aspect, f32 near, f32 far);
void DestroyCamera(Camera *camera);
void CalculateCamera(Camera *camera, s32 width, s32 height);
void ChangeOrientation(Camera *camera);