#include "light.h"
#include <cglm/vec3.h>

// encode the information on scene lights so that the opengl UBO can read them
void encodeLights(LightSource* lights, uint numLights, mat4 view, LightData* dest){
    // make usre to align them as such
    // 0  pos.x 
    // 4  pos.y
    // 8  pos.z
    // 12
    // 16 col.r  
    // 20 col.g
    // 24 col.b
    // 28 power
    for (int i = 0; i < numLights; i++) {
        // precompute multiply by view matrix
        glm_mat4_mulv3(view, lights[i].object.position.raw,1, dest[i].positionCamera );
        glm_vec3_copy(lights[i].colour.raw, dest[i].colour);
        dest[i].power = lights[i].power;
    }

}