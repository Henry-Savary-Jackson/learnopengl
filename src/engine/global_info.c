#include <learnopengl/engine/global_info.h>
#include <learnopengl/engine/camera.h>
#include <learnopengl/engine/light.h>
#include <learnopengl/engine/object.h>
#include <assert.h>
#include <cglm/mat4.h>
#include <cglm/simd/sse2/mat4.h>
#include <cglm/types.h>
#include <glad/glad.h>
#include <stdlib.h>
#include <string.h>

static const uint SHADER_BUFFER_LIGHT_BINDING = 0;
static const uint UNIFORM_VIEW_PROJ_BINDING = 1;
static const uint UNIFORM_MATERIAL_BINDING = 2;
static const uint SHADER_BUFFER_MODELS_BIND = 3;
static const uint SHADER_BUFFER_INV_MODELS_BIND = 4;

static const char *SHADER_BUFFER_LIGHT_NAME = "Lights";
static const char *UNIFORM_VIEW_PROJ_NAME = "ViewProjection";
static const char *UNIFORM_MATERIAL_NAME = "Material";
static const char *SHADER_BUFFER_MODELS_NAME = "ModelTransforms";
static const char *SHADER_BUFFER_INV_MODELS_NAME = "InverseModelTransforms";

void initLightInfoManager(LightInfoManager *manager, uint maxLights) {
  glGenBuffers(1, &(manager->lightsSSBO));
  manager->maxLights = maxLights;
  manager->lights = malloc(sizeof(vec4)+sizeof(LightData)* maxLights);
  manager->numLights = 0;
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, manager->lightsSSBO);
  glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(vec4)+sizeof(LightData) * maxLights, NULL,
               GL_DYNAMIC_DRAW);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void bindLightInfoManager(LightInfoManager *manager,
                          ShaderProgram *shaderProgram) {


  uint modelsIndex = glGetProgramResourceIndex(
      shaderProgram->id, GL_SHADER_STORAGE_BLOCK, SHADER_BUFFER_LIGHT_NAME);
  if (modelsIndex != GL_INVALID_INDEX) {
    glShaderStorageBlockBinding(shaderProgram->id, modelsIndex,
                                SHADER_BUFFER_LIGHT_BINDING);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, SHADER_BUFFER_LIGHT_BINDING,
                     manager->lightsSSBO);
  }
}

void getProjectionManagerView(ProjectionManager* manager, mat4 dest){
  memcpy(dest, manager->projectionBuffer->viewMatrix, sizeof(mat4));
}

void getProjectionManagerProjection(ProjectionManager* manager, mat4 dest){
  memcpy(dest, manager->projectionBuffer->projectionMatrix, sizeof(mat4));
}

void addLightsToManager(LightInfoManager *manager, LightSource *lights,
                        uint numLights, mat4 view) {
  if (manager->numLights + numLights >= manager->maxLights)
    return;
  LightData* lightsArrStart = (LightData*)((float*)manager->lights + 4);
  encodeLights(lights, numLights, view, lightsArrStart + manager->numLights);
  manager->numLights += numLights;
  uint* start = (uint*)(manager->lights);
  start[0] = manager->numLights;
}
void addLightToManager(LightInfoManager *manager, LightSource *lights,
                       mat4 view) {
  addLightsToManager(manager, lights, 1, view);
}
void flushLightManager(LightInfoManager * manager){
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, manager->lightsSSBO);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(vec4) + sizeof(LightData)*manager->numLights, manager->lights );
    manager->numLights = 0;
}

void freeLightManager(LightInfoManager *manager) {
  glDeleteBuffers(1, &(manager->lightsSSBO));
  free(manager->lights);
}

void initTransformMatrixManager(TransformationMatrixManager *manager, uint maxModels) {
  manager->maxModels = maxModels;
  manager->numModels = 0;
  manager->models = malloc(sizeof(mat4)*maxModels);
  manager->invModels = malloc(sizeof(mat4)*maxModels);

  glGenBuffers(1, &(manager->modelMatrixSSBO));
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, manager->modelMatrixSSBO);
  glBufferData(GL_SHADER_STORAGE_BUFFER, maxModels * sizeof(mat4), NULL,
               GL_DYNAMIC_DRAW);

  glGenBuffers(1, &(manager->invModelMatrixSSBO));
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, manager->invModelMatrixSSBO);
  glBufferData(GL_SHADER_STORAGE_BUFFER, maxModels * sizeof(mat4), NULL,
               GL_DYNAMIC_DRAW);

  glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
}

void bindTransformMatrixManager(TransformationMatrixManager *manager,
                            ShaderProgram *shaderProgram) {
  uint modelsIndex = glGetProgramResourceIndex(
      shaderProgram->id, GL_SHADER_STORAGE_BLOCK, SHADER_BUFFER_MODELS_NAME);
  if (modelsIndex != GL_INVALID_INDEX) {
    glShaderStorageBlockBinding(shaderProgram->id, modelsIndex,
                                SHADER_BUFFER_MODELS_BIND);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, SHADER_BUFFER_MODELS_BIND,
                     manager->modelMatrixSSBO);
  }

  uint invModelsIndex =
      glGetProgramResourceIndex(shaderProgram->id, GL_SHADER_STORAGE_BLOCK,
                                SHADER_BUFFER_INV_MODELS_NAME);
  if (invModelsIndex != GL_INVALID_INDEX) {
    glShaderStorageBlockBinding(shaderProgram->id, invModelsIndex,
                                SHADER_BUFFER_INV_MODELS_BIND);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, SHADER_BUFFER_INV_MODELS_BIND,
                     manager->modelMatrixSSBO);
  }
}
void addTransformationToManager(TransformationMatrixManager *manager, mat4 transformation) {
  if (manager->numModels >= manager->maxModels)
    return;
  mat4 modelInverse;
  glm_mat4_inv_fast(transformation, modelInverse);
  // not need to be precise, just very fast
  memcpy(manager->models + manager->numModels, transformation, sizeof(mat4));
  memcpy(manager->invModels + manager->numModels, modelInverse, sizeof(mat4));
  manager->numModels +=1;
}

void flushTransformationManager(TransformationMatrixManager * manager){
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, manager->modelMatrixSSBO);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(mat4)*manager->numModels, manager->models);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, manager->invModelMatrixSSBO);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(mat4)*manager->numModels, manager->invModels);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
    manager->numModels = 0;
}
void freeTransformMatrixManager(TransformationMatrixManager *manager) {
  glDeleteBuffers(1, &(manager->modelMatrixSSBO));
  glDeleteBuffers(1, &(manager->invModelMatrixSSBO));
  free(manager->models);
  free(manager->invModels);
}

void initProjectionMatrixManager(ProjectionManager *manager) {

  glGenBuffers(1, &(manager->protjectionUBO));
  manager->projectionBuffer = malloc(
      sizeof(ProjectionInfo)); // TODO : maybe define a struct for great clarity

  glBindBuffer(GL_UNIFORM_BUFFER, manager->protjectionUBO);
  glBufferData(GL_UNIFORM_BUFFER, sizeof(ProjectionInfo), NULL,
               GL_DYNAMIC_DRAW);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
void setProjectionMatrixManager(ProjectionManager *manager, Camera *camera) {
  getViewMatrix(camera, manager->projectionBuffer->viewMatrix);
  getProjectionMatrix(camera, manager->projectionBuffer->projectionMatrix);
  // precompute projection * view
  glm_mat4_mul(manager->projectionBuffer->projectionMatrix,
               manager->projectionBuffer->viewMatrix,
               manager->projectionBuffer->projectionTimesView);
}

void bindProjectionManagerShader(ProjectionManager *manager,
                                 ShaderProgram *shaderProgram) {
  uint viewProjBlockIndex =
      glGetUniformBlockIndex(shaderProgram->id, UNIFORM_VIEW_PROJ_NAME);
  assert(viewProjBlockIndex != GL_INVALID_INDEX);
  glUniformBlockBinding(shaderProgram->id, viewProjBlockIndex,
                        UNIFORM_VIEW_PROJ_BINDING);
  // link ubo to binding point
  glBindBufferBase(GL_UNIFORM_BUFFER, UNIFORM_VIEW_PROJ_BINDING,
                   manager->protjectionUBO);
}

void flushProjectionMatrixManager(ProjectionManager *manager) {
    glBindBuffer(GL_UNIFORM_BUFFER, manager->protjectionUBO);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(ProjectionInfo), manager->projectionBuffer);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void freeProjectionManager(ProjectionManager *manager) {
  glBindBufferBase(GL_UNIFORM_BUFFER, UNIFORM_VIEW_PROJ_BINDING, 0);
  glDeleteBuffers(1, &(manager->protjectionUBO));
  free(manager->projectionBuffer);
}

void initMaterialManager(MaterialManager *manager) {
  glGenBuffers(1, &(manager->materialUBO));
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, manager->materialUBO);
  glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(Material), NULL, GL_DYNAMIC_DRAW);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
  manager->material = NULL;
}
void setMaterialManagerMaterial(MaterialManager *manager, Material *material) {
  manager->material = material;
}
void bindMaterialManagerShader(MaterialManager *manager,
                               ShaderProgram *shaderProgram) {
  uint materialIndex =
      glGetProgramResourceIndex(shaderProgram->id, GL_SHADER_STORAGE_BLOCK,UNIFORM_MATERIAL_NAME);

  if (materialIndex != GL_INVALID_INDEX) {
    glShaderStorageBlockBinding(shaderProgram->id, materialIndex,
                          UNIFORM_MATERIAL_BINDING);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, UNIFORM_MATERIAL_BINDING,
                     manager->materialUBO);
  }
}
void flushMaterialManager(MaterialManager* manager){
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, manager->materialUBO);
    glBufferSubData(GL_SHADER_STORAGE_BUFFER,  0,sizeof(Material), manager->material);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER,0);
}
void freeMaterialManager(MaterialManager *manager) {
  glDeleteBuffers(1, &(manager->materialUBO));
}
