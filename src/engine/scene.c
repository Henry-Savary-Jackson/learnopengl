#include "learnopengl/engine/batch_render.h"
#include "learnopengl/engine/camera.h"
#include "learnopengl/engine/global_info.h"
#include "learnopengl/engine/object.h"
#include "learnopengl/engine/shader.h"
#include "learnopengl/utils/arraylist.h"
#include <learnopengl/engine/scene.h>
#include <setjmp.h>

void initScene(Scene *scene, Camera *camera, uint maxShaders, uint maxObjects,
               uint maxLights) {
  scene->camera = camera;
  scene->maxLights = maxLights;
  scene->maxObjects = maxObjects;
  scene->maxShaders = maxShaders;
  scene->maxTextures = MAX_TEXTURES;
  initProjectionMatrixManager(&scene->projectionManager);
  initLightInfoManager(&scene->lightManager, maxLights);
  initBatchRenderer(&scene->dynamicRenderer, maxObjects * 10, maxObjects * 300,
                    maxObjects);
  initBatchRenderer(&scene->staticRenderer, maxObjects * 10, maxObjects * 300,
                    maxObjects);
  scene->numObjects = 0;
  scene->numShaders = 0;
  scene->shaderObjects = NULL;
  scene->gameObjects = NULL;
  scene->numLights = 0;
}

void addObjectToScene(Scene *scene, GameObject *object, uint objectType) {
  // if over, flush
  if (scene->numObjects >= scene->maxObjects)
    return;

  addObjectToCorrectShaderList(scene, object, objectType);
  scene->gameObjects[scene->numObjects] = object;
  scene->numObjects++;
}

void addObjectToCorrectShaderList(Scene *scene, GameObject *object,
                                  uint objectType) {
  int i = 0;
  for (; i < scene->numShaders; i++) {
    if (scene->shaderObjects[i].shaderProgram->id == object->shaderProgram->id)
      goto add_to_list;
  }
  assert(i < scene->maxShaders);
  if (i == scene->numShaders) {
    initShaderObjects(&scene->shaderObjects[i], object->shaderProgram);
    scene->numShaders++;
  }

add_to_list:
  ArrayList_p *arrayList = objectType == STATIC_OBJ
                               ? &scene->shaderObjects[i].staticObjects
                               : &scene->shaderObjects[i].dynamicObjects;
  append_p(arrayList, object);
}

void initShaderObjects(ShaderObjects *shaderObjects, ShaderProgram *program) {
  shaderObjects->shaderProgram = program;
  initArrayList_p(&shaderObjects->dynamicObjects, 50, 10);
  initArrayList_p(&shaderObjects->staticObjects, 50, 10);
}

void addLightToScene(Scene *scene, LightSource *light, uint objectType) {
  addObjectToScene(scene, &light->object, objectType);
  scene->lights[scene->numLights] = light;
  scene->numLights++;
}

void preLoop(Scene *scene) {
  flushLightManager(&scene->lightManager);
  flushProjectionMatrixManager(&scene->projectionManager);
}

void renderLoop(Scene *scene) {

  for (int i = 0; i < scene->numLights; i++) {
    mat4 view;
    getViewMatrix(scene->camera, view);
    addLightToManager(&scene->lightManager, scene->lights[i], view);
  }
  // send data to ogl buffers
  setProjectionMatrixManager(&scene->projectionManager, scene->camera);
  flushLightManager(&scene->lightManager);

  for (int i = 0; i < scene->numShaders; i++) {
    ShaderObjects *shaderObjects = &scene->shaderObjects[i];
    ShaderProgram *shaderProgram = shaderObjects->shaderProgram;
    useShaderProgram(shaderProgram);

    // bind the location in the shader to buffers
    bindProjectionManagerShader(&scene->projectionManager, shaderProgram);

    bindLightInfoManager(&scene->lightManager, shaderProgram);

    for (int j = 0; j < shaderObjects->dynamicObjects.length; j++) {
      renderObjectBatch(&scene->dynamicRenderer,
                        shaderObjects->dynamicObjects.ptr[j]);
    }

    flush(&scene->dynamicRenderer);
  }
  unbindShaderProgram();
}

void freeScene(Scene *scene) {
  for (int i = 0; i < scene->numShaders; i++) {
    destroyArrayList_p(&scene->shaderObjects[i].dynamicObjects);
    destroyArrayList_p(&scene->shaderObjects[i].staticObjects);
  }
  freeBatchRenderer(&scene->dynamicRenderer);
  freeBatchRenderer(&scene->staticRenderer);
  freeLightManager(&scene->lightManager);
  freeProjectionManager(&scene->projectionManager);
}
