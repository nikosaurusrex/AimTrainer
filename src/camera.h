#include "common.h"

#include "glm/glm.hpp"

struct camera_t {
    glm::mat4   projection;
    glm::mat4   view;
    glm::vec3   position;

    f32         fov;
    f32         aspect;
    f32         near = 0.0f;
    f32         far = 0.0f;

    f64         yaw;
    f64         pitch;
};

camera_t    *CreateCamera(f32 fov, f32 aspect, f32 near, f32 far);
void        DestroyCamera(camera_t *camera);
void        CalculateCamera(camera_t *camera, s32 width, s32 height);
void        ChangeOrientation(camera_t *camera);