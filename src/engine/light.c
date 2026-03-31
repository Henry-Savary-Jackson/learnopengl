#include <learnopengl/engine/light.h>
#include <cglm/vec3.h>
#include <learnopengl/utils/utils.h>
float getLightRadius(LightSource* lightData){
    const float constTerm = 1;
    const float linearTerm= 0.7;
    const float quadTerm  = 1.8;
    const float maxComp = MAX(MAX(lightData->colour.r, lightData->colour.g), lightData->colour.b);
    return (-linearTerm + sqrt(linearTerm*linearTerm- 4 * quadTerm * (constTerm- lightData->power * 256/5)))/(2*quadTerm);
}

// encode the information on scene lights so that the opengl UBO can read them
void encodeLights(LightSource* lights, uint numLights, LightData* dest){
    for (int i = 0; i < numLights; i++) {
        // precompute multiply by view matrix
        glm_vec3_copy(lights[i].object.position.raw, dest[i].position);
        glm_vec3_copy(lights[i].colour.raw, dest[i].colour);
        dest[i].power = lights[i].power;
    }
}
