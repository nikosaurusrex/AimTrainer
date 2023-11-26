#include "camera.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

camera_t *CreateCamera(f32 fov, f32 aspect, f32 near, f32 far) {
    camera_t *camera = (camera_t *) malloc(sizeof(camera_t));

    camera->fov 		= fov;
    camera->aspect 		= aspect;
    camera->near 		= near;
    camera->far 		= far;
    camera->yaw 		= 0;
    camera->pitch		= 0;
    camera->position 	= {0, 0, 0};

    return camera;
}

void DestroyCamera(camera_t *camera) {
    free(camera);
}

void CalculateCamera(camera_t *camera, s32 width, s32 height) {
    camera->aspect = (f32)width / (f32)height;  

    camera->projection = glm::perspective(glm::radians(camera->fov), camera->aspect, camera->near, camera->far);

    ChangeOrientation(camera);
}

void ChangeOrientation(camera_t *camera) {
    glm::quat orientation = glm::quat(glm::vec3(glm::radians(-camera->pitch), glm::radians(-camera->yaw), 0.0f));

    camera->view = glm::translate(glm::mat4(1.0f), camera->position) * glm::toMat4(orientation);;
    camera->view = glm::inverse(camera->view); 
}