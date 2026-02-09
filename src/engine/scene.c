#include "learnopengl/engine/batch_render.h"
#include "learnopengl/engine/camera.h"
#include "learnopengl/engine/global_info.h"
#include <learnopengl/engine/skybox.h>
#include "learnopengl/engine/mesh.h"
#include "learnopengl/engine/object.h"
#include "learnopengl/engine/shader.h"
#include "learnopengl/engine/skybox.h"
#include <glad/glad.h>
#include <learnopengl/engine/render_context.h>
#include <learnopengl/engine/scene.h>
#include <stdio.h>
#include <stdlib.h>

void initScene(Scene *scene, Camera *camera, uint maxShaders, uint maxObjects,
               uint maxLights) {
  scene->camera = camera;
  scene->maxLights = maxLights;
  scene->maxObjects = maxObjects;
  scene->numTextures = 0;
  scene->maxTextures = 100;
  initRenderingContext(&scene->renderingContext, camera, maxLights, 100,
                       scene->maxTextures);
  initBatchRenderer(&scene->dynamicRenderer, maxObjects * 50000,
                    maxObjects * 1000000, maxObjects, maxShaders);
  initBatchRenderer(&scene->staticRenderer, maxObjects * 50000,
                    maxObjects * 1000000, maxObjects, maxShaders);
  scene->numObjectsDynamic = 0;
  scene->numObjectsStatic = 0;
  scene->lights = malloc(sizeof(LightSource) * scene->maxLights);
  scene->dynamicObjects = malloc(sizeof(GameObject *) * scene->maxObjects);
  scene->staticObjects = malloc(sizeof(GameObject *) * scene->maxObjects);
  scene->numLights = 0;
  scene->numShaders = 0;
  scene->maxShaders = maxShaders;
  scene->shaders = malloc(sizeof(ShaderProgram *) * maxShaders);
  scene->textureIndicesDynamic = malloc(sizeof(uint) * scene->maxObjects);
  scene->textureIndicesStatic = malloc(sizeof(uint) * scene->maxObjects);
  scene->skybox = NULL;
}

void addObjectToScene(Scene *scene, GameObject *object, uint objectType) {
  if (scene->numObjectsDynamic + scene->numObjectsStatic >= scene->maxObjects)
    return;
  if (scene->numShaders >= scene->maxShaders)
    return;

  GameObject **objectList =
      objectType == STATIC_OBJ ? scene->staticObjects : scene->dynamicObjects;

  // either increase the static of dynamic object count
  uint *index = objectType == STATIC_OBJ ? &scene->numObjectsStatic
                                         : &scene->numObjectsDynamic;
  objectList[*index] = object;

  if (object->numTextures > 0) {
    uint* texuteIndexList = objectType == STATIC_OBJ ? scene->textureIndicesStatic : scene->textureIndicesDynamic;
    uint textureIndex = addObjectTexture(&scene->renderingContext.textureManager, object);
    texuteIndexList[*index] = textureIndex;
  }

  (*index)++;

  // look for shader in list
  uint shaderIndex = 0;
  for (; shaderIndex < scene->numShaders; shaderIndex++)
    if (scene->shaders[shaderIndex]->id == object->shaderProgram->id)
      break;

  if (shaderIndex == scene->numShaders) {
    // add new unique shader to list and give it scene context
    scene->shaders[shaderIndex] = object->shaderProgram;
    scene->numShaders++;
    attachContextToShader(&scene->renderingContext, object->shaderProgram);
  }


}

void addLightToScene(Scene *scene, LightSource *light, uint objectType) {
  addObjectToScene(scene, &light->object, objectType);
  scene->lights[scene->numLights] = light;
  scene->numLights++;
}

void preLoop(Scene *scene) {

  // uint result;
  // for (int i = 0; i < scene->numObjectsStatic; i++) {
  //   if ((result = copyObjectToBuffer(&scene->staticRenderer,
  //                                    scene->staticObjects[i])) !=
  //       BATCH_RENDER_OK)
  //     break;
  // }
  // // sendDataToGPU(&scene->staticRenderer);
  // sendDataToGPU(&scene->dynamicRenderer);
}

void renderLoop(Scene *scene) {
  for (int i = 0; i < scene->numLights; i++)
    addLightToContext(&scene->renderingContext, scene->lights[i]);

  for (int i = 0; i < scene->numObjectsDynamic; i++)
    addObjectToContext(&scene->renderingContext, scene->dynamicObjects[i]);


  updateContext(&scene->renderingContext);

  if (scene->skybox != NULL)
    renderSkybox(scene->skybox);

  for (int i = 0; i < scene->numObjectsDynamic; i++)
    renderObjectBatch(&scene->dynamicRenderer, scene->dynamicObjects[i], scene->textureIndicesDynamic[i]);

  flush(&scene->dynamicRenderer);
//
}

void setSkybox(Scene *scene, Skybox *skybox, Texture *texture,
               Sampler *sampler) {
  uint index =
      addTexture(&scene->renderingContext.textureManager, texture, sampler);
  scene->numTextures = scene->renderingContext.textureManager.numHandles;
  scene->skybox = skybox;
  attachContextToShader(&scene->renderingContext, skybox->shaderProgram);
  setSkyboxTextureIndex(skybox, index);
}

void freeTextures(Scene *scene) {
}

void freeScene(Scene *scene) {
  freeBatchRenderer(&scene->dynamicRenderer);
  freeBatchRenderer(&scene->staticRenderer);
  freeRenderingContext(&scene->renderingContext);
  freeTextures(scene);
  free(scene->textureIndicesDynamic);
  free(scene->textureIndicesStatic);
  free(scene->dynamicObjects);
  free(scene->staticObjects);
  free(scene->shaders);
}
