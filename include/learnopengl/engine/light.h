#pragma  once
#include <cglm/types-struct.h>
#include <stdalign.h>
#include <learnopengl/engine/object.h>

typedef struct  {
    vec3 positionCamera; // relative to camera
    alignas(16) vec3 colour;
    alignas(16) float power;
} LightData;
// alignas is to ensure it fits with def in shader that uses std430

void encodeLights(LightSource* lights, uint numLights, mat4 view, LightData* dest);
