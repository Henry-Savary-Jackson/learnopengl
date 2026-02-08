#pragma once
#include <cglm/types.h>
#include <learnopengl/engine/camera.h>
#include <learnopengl/engine/light.h>
#include <learnopengl/engine/object.h>
#include <learnopengl/engine/shader.h>
#include <stdint.h>
#include <sys/types.h>

typedef struct {

  uint lightsSSBO;
  uint numLights;
  uint maxLights;
  LightData *lights;

} LightInfoManager;

typedef struct {

  mat4 *models;
  mat4 *invModels;

  uint modelMatrixSSBO;
  uint invModelMatrixSSBO;
  uint maxModels;
  uint numModels;

} TransformationMatrixManager;
// users should be able to change and go above the maximum number of models
// by calling glBufferData when needed

#define MAX_TEXTURES 48
#define MAX_LIGHTS 50
#define MAX_TEXTURES_EXCEEDED -2

typedef struct {
  uint protjectionUBO;
  ProjectionInfo *projectionBuffer;
} ProjectionManager;

typedef struct {
  uint materialUBO;
  MaterialData* materialsBuffer;
  uint* ids;
  uint numMaterials;
  uint maxMaterials;
} MaterialManager;

typedef struct {
  uint numHandles;
  uint maxTextures;
  uint64_t *handles;
  uint textureHandleSSBO;
} TextureManager;

void initLightInfoManager(LightInfoManager *manager, uint maxLights);
void bindLightInfoManager(LightInfoManager *manager, ShaderProgram *shader);
void addLightsToManager(LightInfoManager *manager, LightSource *lights,
                        uint numLight, mat4 view);
void addLightToManager(LightInfoManager *manager, LightSource *lights,
                       mat4 view);
void flushLightManager(LightInfoManager *manager);
void freeLightManager(LightInfoManager *manager);

void initProjectionMatrixManager(ProjectionManager *manager);
void setProjectionMatrixManager(ProjectionManager *manager, Camera *camera);
void getProjectionManagerView(ProjectionManager *manager, mat4 dest);
void getProjectionManagerProjection(ProjectionManager *manager, mat4 dest);
void bindProjectionManagerShader(ProjectionManager *manager,
                                 ShaderProgram *shaderProgram);
void flushProjectionMatrixManager(ProjectionManager *manager);
void freeProjectionManager(ProjectionManager *manager);

void initMaterialManager(MaterialManager *manager, uint maxMaterials);
void addMaterialToManager(MaterialManager *manager, Material *material) ;
void bindMaterialManagerShader(MaterialManager *manager,
                               ShaderProgram *shaderProgram);
void flushMaterialManager(MaterialManager *manager);
void freeMaterialManager(MaterialManager *manager);

void initTextureManager(TextureManager *manager, uint maxTextures);
uint addObjectTexture(TextureManager *manager, GameObject *object);

void bindTextureManagerToShader(TextureManager *manager,
                                ShaderProgram *shaderProgram);
void flushTextureManager(TextureManager *manager);
void freeTextureManager(TextureManager *manager);
