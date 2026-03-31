#pragma  once
#include <cglm/types-struct.h>
#include <stdalign.h>
#include <learnopengl/engine/object.h>

typedef struct  {
    vec3 position; // relative to camera
    alignas(16) vec3 colour;
    alignas(16) float power;
} LightData;
// alignas is to ensure it fits with def in shader that uses std430

static const float MIN_INTENSITY = 5.0/255;

void encodeLights(LightSource* lights, uint numLights,  LightData* dest);
float getLightRadius(LightSource* lightData);
