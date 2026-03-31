#pragma once

#include <cglm/types-struct.h>
#include <sys/types.h>


typedef struct{
    vec3s position;
    vec2s angle;
    float fov;
    uint width;
    uint height;
    vec3s up;
    vec3s direction;
} Camera ;

typedef struct  {
    mat4 viewMatrix;
    mat4 projectionMatrix;
    mat4 projectionTimesView;
    vec3s cameraPos;
    alignas(16) vec3s cameraDirection ;
    alignas(16) vec2s screenSize;
} ProjectionInfo;

void initCamera( Camera * camera, vec3s position, float fov, uint width, uint height);

void getViewMatrix(Camera* camera, mat4 dest);

void getProjectionMatrix(Camera* camera, mat4 dest);

void translateCamera(Camera* camera, vec3 translation);
void rotateCamera(Camera* camera, vec2 rotation);
void setCameraDims(Camera* camera, int width, int height);

void setCameraPos(Camera* camera, vec3s newPos);
void setCameraAngles(Camera* camera, vec2s newAngle);

void moveCameraFront(Camera* camera, float amount);
void moveCameraSide(Camera* camera, float amount);

void moveCameraSide(Camera *camera, float amount);

void moveCameraVertical(Camera* camera, float amount);
