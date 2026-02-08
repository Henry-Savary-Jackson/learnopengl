#include "learnopengl/engine/batch_render.h"
#include "learnopengl/engine/camera.h"
#include "learnopengl/engine/object.h"
#include "learnopengl/engine/shader.h"
#include <glad/glad.h>
#include <learnopengl/engine/render_context.h>
#include <learnopengl/engine/scene.h>
#include <stdlib.h>

void initScene(Scene *scene, Camera *camera, uint maxShaders, uint maxObjects,
               uint maxLights) {
  scene->camera = camera;
  scene->maxLights = maxLights;
  scene->maxObjects = maxObjects;
  initRenderingContext(&scene->renderingContext, camera, maxLights);
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

  scene->numTextures = 0;
  scene->maxTextures = 100;
  scene->textureHandles = malloc(sizeof(uint) * scene->maxTextures);
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

  if (object->numTextures > 0) {
    uint textureIndex = 0;
    uint64_t handle = glGetTextureSamplerHandleARB(object->textures->glID,
                                                   object->sampler->glID);

    for (; textureIndex < scene->maxTextures; textureIndex++)
      if (scene->textureHandles[textureIndex] == handle)
        break;

    if (textureIndex == scene->numTextures) {
      scene->textureHandles[textureIndex] = handle;
      glMakeTextureHandleResidentARB(handle);
      scene->numTextures++;
    }
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
    addObjectToContext(&scene->renderingContext, scene->dynamicObjects[i] );

  updateContext(&scene->renderingContext);

  for (int i = 0; i < scene->numObjectsDynamic; i++)
    renderObjectBatch(&scene->dynamicRenderer, scene->dynamicObjects[i]);

 flush(&scene->dynamicRenderer);
}

void freeTextures(Scene *scene) {
  for (uint textureIndex = 0; textureIndex < scene->numTextures; textureIndex++)
    glMakeTextureHandleNonResidentARB(scene->textureHandles[textureIndex]);
}

void freeScene(Scene *scene) {
  freeBatchRenderer(&scene->dynamicRenderer);
  freeBatchRenderer(&scene->staticRenderer);
  freeRenderingContext(&scene->renderingContext);
  freeTextures(scene);
  free(scene->textureHandles);
  free(scene->dynamicObjects);
  free(scene->staticObjects);
  free(scene->shaders);
}
