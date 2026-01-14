
#include <cglm/types-struct.h>
#include "object.h"
typedef struct {
    vec3s position;
    float pad_1;
    vec3s colour;
    float pad_2;
    float intensity;
} LightSource;

typedef struct {
    float ambientCoeffient;
    float diffuseCoeffient;
    float specularCoefficient;
    float __padd_1;
    vec3s colour;
    float __padd_2;
} Material;

typedef struct{
    GameObject object;
    LightSource* light;
} LightObject;



