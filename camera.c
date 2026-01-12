#include "camera.h"
#include <cglm/cam.h>
#include <cglm/cglm.h>
#include <cglm/struct/vec2.h>
#include <cglm/types-struct.h>
#include <cglm/types.h>
#include <cglm/util.h>
#include <cglm/vec2.h>
#include <cglm/vec3.h>
#include <math.h>

void setDirection(Camera *camera) {
  float yaw_rad = glm_rad(camera->angle.x);
  float pitch_rad = glm_rad(camera->angle.y);
  camera->direction = (vec3s){cos(pitch_rad) * cos(yaw_rad), sin(pitch_rad),
                              sin(yaw_rad) * cos(pitch_rad)};
  glm_normalize(camera->direction.raw);
    
}
void initCamera(Camera *camera, vec3s position, float fov, uint width,
                uint height) {
  camera->position = position;
  camera->fov = fov;
  camera->width = width;
  camera->height = height;
  camera->angle = (vec2s){-90.f, 0};
  camera->up = (vec3s){0, 1.f, 0};
  setDirection(camera);
}

void getViewMatrix(Camera *camera, mat4 dest) {
    setDirection(camera);
  vec3 lookAt;
  glm_vec3_add(camera->position.raw, camera->direction.raw, lookAt);
  glm_lookat(camera->position.raw, lookAt, camera->up.raw, dest);
}

void getProjectionMatrix(Camera *camera, mat4 dest) {
  float aspect = camera->width / camera->height;
  glm_perspective(camera->fov, aspect, 0.1f, 100.f, dest);
}

void translateCamera(Camera *camera, vec3 translation) {
  glm_vec3_add(camera->position.raw, translation, camera->position.raw);
}
void rotateCamera(Camera *camera, vec2 rotation) {
  glm_vec2_add(camera->angle.raw, rotation, camera->angle.raw);
  setDirection(camera);
}
void setCameraPos(Camera *camera, vec3s newPos) { camera->position = newPos; }
void setCameraAngles(Camera *camera, vec2s newAngle) {
  camera->angle = newAngle;
  setDirection(camera);
}

void setCameraDims(Camera *camera, int width, int height) {
  camera->width = width;
  camera->height = height;
}

void moveCameraFront(Camera *camera, float amount) { vec3 scaled_dir;
    glm_vec3_scale(camera->direction.raw, amount, scaled_dir);
    translateCamera(camera, scaled_dir);
 }
void moveCameraSide(Camera *camera, float amount){
    vec3 scaled_dir;
    glm_cross( camera->direction.raw,camera->up.raw, scaled_dir);
    glm_normalize( scaled_dir);
    glm_vec3_scale(scaled_dir, amount, scaled_dir);
    translateCamera(camera, scaled_dir);
}

void moveCameraVertical(Camera* camera, float amount){
    vec3 scaled_vert;
    glm_vec3_scale(camera->up.raw, amount, scaled_vert);
    translateCamera(camera, scaled_vert);
}