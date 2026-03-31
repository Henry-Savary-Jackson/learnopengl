#include "learnopengl/engine/mesh.h"
#include "learnopengl/engine/shader.h"
#include "learnopengl/engine/textures.h"
#include <assert.h>
#include <cglm/mat4.h>
#include <cglm/simd/sse2/mat4.h>
#include <cglm/types.h>
#include <glad/glad.h>
#include <learnopengl/engine/camera.h>
#include <learnopengl/engine/global_info.h>
#include <learnopengl/engine/light.h>
#include <learnopengl/engine/object.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

static const uint SHADER_BUFFER_LIGHT_BINDING = 0;
static const uint UNIFORM_VIEW_PROJ_BINDING = 1;
static const uint UNIFORM_MATERIAL_BINDING = 2;
static const uint SHADER_BUFFER_MODELS_BIND = 3;
static const uint SHADER_BUFFER_INV_MODELS_BIND = 4;
static const uint SHADER_TEXTURE_BINDING = 5;

static const char *SHADER_BUFFER_LIGHT_NAME = "Lights";
static const char *UNIFORM_VIEW_PROJ_NAME = "ViewProjection";
static const char *UNIFORM_MATERIAL_NAME = "Materials";
static const char *SHADER_BUFFER_MODELS_NAME = "ModelTransforms";
static const char *SHADER_BUFFER_INV_MODELS_NAME = "InverseModelTransforms";
static const char *SHADER_BUFFER_TEXTURE_NAME = "Textures";
static const char *SKYBOX_UNIFORM_UNIFORM_NAME = "Skybox";

void initLightInfoManager(LightInfoManager *manager, uint maxLights) {
  glGenBuffers(1, &(manager->lightsSSBO));
  manager->maxLights = maxLights;
  manager->lights = malloc(sizeof(vec4) + sizeof(LightData) * maxLights);
  manager->numLights = 0;
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

void getProjectionManagerView(ProjectionManager *manager, mat4 dest) {
  memcpy(dest, manager->projectionBuffer->viewMatrix, sizeof(mat4));
}

void getProjectionManagerProjection(ProjectionManager *manager, mat4 dest) {
  memcpy(dest, manager->projectionBuffer->projectionMatrix, sizeof(mat4));
}

void addLightsToManager(LightInfoManager *manager, LightSource *lights,
                        uint numLights) {
  if (manager->numLights + numLights >= manager->maxLights)
    return;
  LightData *lightsArrStart = (LightData *)((float *)manager->lights + 4);
  encodeLights(lights, numLights, lightsArrStart + manager->numLights);
  manager->numLights += numLights;
}
void addLightToManager(LightInfoManager *manager, LightSource *lights
                       ) {
  addLightsToManager(manager, lights, 1 );
}
void flushLightManager(LightInfoManager *manager) {

  // make sure the first 4 bytes of buffer contain the total number of lights
  uint *start = (uint *)(manager->lights);
  *start = manager->numLights;
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, manager->lightsSSBO);
  glBufferData(GL_SHADER_STORAGE_BUFFER,
               sizeof(vec4) + sizeof(LightData) * manager->numLights,
               manager->lights, GL_STATIC_DRAW);
  manager->numLights = 0;
}

void freeLightManager(LightInfoManager *manager) {
  glDeleteBuffers(1, &(manager->lightsSSBO));
  free(manager->lights);
}

void initProjectionMatrixManager(ProjectionManager *manager) {

  glGenBuffers(1, &(manager->protjectionUBO));
  manager->projectionBuffer = malloc(
      sizeof(ProjectionInfo)); // TODO : maybe define a struct for great clarity

  // glBindBuffer(GL_UNIFORM_BUFFER, manager->protjectionUBO);
  // glBufferData(GL_UNIFORM_BUFFER, sizeof(ProjectionInfo), NULL,
  //              GL_DYNAMIC_DRAW);
  // glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
void setProjectionMatrixManager(ProjectionManager *manager, Camera *camera) {
  getViewMatrix(camera, manager->projectionBuffer->viewMatrix);
  getProjectionMatrix(camera, manager->projectionBuffer->projectionMatrix);
  // precompute projection * view
  glm_mat4_mul(manager->projectionBuffer->projectionMatrix,
               manager->projectionBuffer->viewMatrix,
               manager->projectionBuffer->projectionTimesView);
  manager->projectionBuffer->cameraPos = camera->position;
  manager->projectionBuffer->cameraDirection = camera->direction;
  manager->projectionBuffer->screenSize = (vec2s){camera->width, camera->height};
}

void bindProjectionManagerShader(ProjectionManager *manager,
                                 ShaderProgram *shaderProgram) {
  uint viewProjBlockIndex =
      glGetUniformBlockIndex(shaderProgram->id, UNIFORM_VIEW_PROJ_NAME);
  if (viewProjBlockIndex != GL_INVALID_INDEX) {
    glUniformBlockBinding(shaderProgram->id, viewProjBlockIndex,
                          UNIFORM_VIEW_PROJ_BINDING);
    // link ubo to binding point
    glBindBufferBase(GL_UNIFORM_BUFFER, UNIFORM_VIEW_PROJ_BINDING,
                     manager->protjectionUBO);
  }
}

void flushProjectionMatrixManager(ProjectionManager *manager) {
  glBindBuffer(GL_UNIFORM_BUFFER, manager->protjectionUBO);
  glBufferData(GL_UNIFORM_BUFFER, sizeof(ProjectionInfo),
               manager->projectionBuffer, GL_STATIC_DRAW);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void freeProjectionManager(ProjectionManager *manager) {
  glBindBufferBase(GL_UNIFORM_BUFFER, UNIFORM_VIEW_PROJ_BINDING, 0);
  glDeleteBuffers(1, &(manager->protjectionUBO));
  free(manager->projectionBuffer);
}

void initTextureManager(TextureManager *manager, uint maxTextures) {
  glGenBuffers(1, &(manager->textureHandleSSBO));
  manager->numHandles = 0;
  manager->handles = malloc(sizeof(int64_t)*2 * maxTextures);
  manager->maxTextures = maxTextures;
}

uint addObjectTexture(TextureManager *manager, GameObject *object) {
  if (object->numTextures <= 0)
    return TEXTURE_UNDEFINED_INDEX;

  return addTexture(manager, object->textures, object->sampler);
}

uint addTexture(TextureManager *manager, Texture *texture, Sampler *sampler) {

  uint64_t handle = glGetTextureSamplerHandleARB(texture->glID, sampler->glID);
  uint index = 0;
  for (; index < manager->numHandles; index++)
    if (handle == manager->handles[index<<1])
      break;

  if (index >= manager->maxTextures)
    return TEXTURE_UNDEFINED_INDEX;

  if (index == manager->numHandles) {
    manager->handles[index<<1] = handle;
    manager->numHandles++;
    glMakeTextureHandleResidentARB(handle);
  }

  return index;
}

void bindTextureManagerToShader(TextureManager *manager,
                                ShaderProgram *shaderProgram) {

  uint texturesIndex =
      glGetUniformBlockIndex(shaderProgram->id ,SHADER_BUFFER_TEXTURE_NAME);

  if (texturesIndex != GL_INVALID_INDEX) {
    glUniformBlockBinding(shaderProgram->id, texturesIndex,
                          SHADER_TEXTURE_BINDING);
    glBindBufferBase(GL_UNIFORM_BUFFER, SHADER_TEXTURE_BINDING,
                     manager->textureHandleSSBO);
  }
}

void flushTextureManager(TextureManager *manager) {
  glBindBuffer(GL_UNIFORM_BUFFER, manager->textureHandleSSBO);
  glBufferData(GL_UNIFORM_BUFFER, sizeof(uint64_t)*2 * manager->numHandles,
               manager->handles,
               GL_STATIC_DRAW);
}

void freeTextureManager(TextureManager *manager) {
  glDeleteBuffers(1, &manager->textureHandleSSBO);
  for (int i = 0 ; i < manager->numHandles; i++)
    glMakeTextureHandleNonResidentARB(manager->handles[i<<1]);
  free(manager->handles);
}

void initMaterialManager(MaterialManager *manager, uint maxMaterials) {
  manager->maxMaterials = maxMaterials;
  manager->numMaterials = 0;
  glGenBuffers(1, &manager->materialUBO);
  manager->materialsBuffer =
      malloc(sizeof(MaterialData) * manager->maxMaterials);
  manager->ids = malloc(sizeof(uint) * maxMaterials);
}

void addMaterialToManager(MaterialManager *manager, Material *material, TextureManager* textureManager) {
  uint materialIndex = 0;
  for (; materialIndex < manager->numMaterials; materialIndex++)
    if (manager->ids[materialIndex] == material->id)
      break;

  if (materialIndex == manager->numMaterials) {
    MaterialData data;
    setMaterialData(manager->materialsBuffer + manager->numMaterials, material,  textureManager);
    manager->ids[manager->numMaterials] = material->id;
    manager->numMaterials++;
  }
}
void bindMaterialManagerShader(MaterialManager *manager,
                               ShaderProgram *shaderProgram) {
  uint materialsIndex =
      glGetUniformBlockIndex(shaderProgram->id, UNIFORM_MATERIAL_NAME);

  if (materialsIndex != GL_INVALID_INDEX) {
    glUniformBlockBinding(shaderProgram->id, materialsIndex,
                          UNIFORM_MATERIAL_BINDING);
    glBindBufferBase(GL_UNIFORM_BUFFER, UNIFORM_MATERIAL_BINDING,
                     manager->materialUBO);
  }
}

void flushMaterialManager(MaterialManager *manager) {
  glBindBuffer(GL_UNIFORM_BUFFER, manager->materialUBO);
  glBufferData(GL_UNIFORM_BUFFER, sizeof(MaterialData) * manager->numMaterials,
               manager->materialsBuffer, GL_STATIC_DRAW);
}

void freeMaterialManager(MaterialManager *manager) {
  glDeleteBuffers(1, &(manager->materialUBO));
  free(manager->materialsBuffer);
  free(manager->ids);
}
