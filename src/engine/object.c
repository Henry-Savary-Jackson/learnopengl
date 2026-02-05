#include <cglm/affine.h>
#include <cglm/vec3.h>
#include <learnopengl/engine/object.h>
#include <cglm/mat4.h>
#include <cglm/vec4.h>
#include <string.h>


void initGameObject(GameObject* object){
    object->mesh = NULL;
    object->numTextures = 0;
    object->textures= NULL;
    object->shaderProgram = NULL;
    object->rotation = (vec3s){0,0,0};
    object->position = (vec3s){0,0,0};
    object->scale = (vec3s){1,1,1};
}

void setMeshGameObject(GameObject* object, Mesh* mesh){
    object->mesh =mesh;
}

void setTexturesGameObject(GameObject* object, Texture* textures, int numTextures){
    object->textures = textures;
    object->numTextures = numTextures;
}

void setShaderProgramGameObject(GameObject* object, ShaderProgram* shaderProgram){
    object->shaderProgram= shaderProgram;
}

void setSampler(GameObject* object, Sampler* sampler){
    object->sampler = sampler;
}

void getModelMatrix(GameObject* object, mat4 dest){
    mat4 init = GLM_MAT4_IDENTITY_INIT;
    mat4 yrot = GLM_MAT4_IDENTITY_INIT ;
    mat4 xrot = GLM_MAT4_IDENTITY_INIT ;
    glm_mat4_identity(dest);
    glm_translate(init, object->position.raw);
    glm_rotate_y(init, object->rotation.x,xrot);
    glm_rotate_x(xrot, object->rotation.y,yrot);
    glm_rotate_z(yrot, object->rotation.z,dest);
    glm_scale(dest, object->scale.raw);
}

void rotateGameObject(GameObject* object, vec3 rotation){
    object->rotation.x= rotation[0];
    object->rotation.y= rotation[1];
    object->rotation.z= rotation[2];
}
void translateGameObject(GameObject* object, vec3 translation){
    glm_vec3_add(object->position.raw, translation, object->position.raw);
}

void rotateGameObjectAround(GameObject* object, float angle, vec3 axis , vec3 pivot){
    mat4 result= GLM_MAT4_IDENTITY_INIT;
    glm_rotate_at(result, pivot ,  angle, axis);
    glm_mat4_mulv3(result, object->position.raw, 1, object->position.raw);
}

void scaleGameObject(GameObject* object, vec3 scale){
    memcpy(object->scale.raw, scale, sizeof(vec3));
}

void scaleGameObjectFloat(GameObject* object, float scale){
    glm_vec3_scale(object->scale.raw, scale, object->scale.raw);
}

void setMaterialGameObject(GameObject* object, Material* material){
    object->material = material;
}
