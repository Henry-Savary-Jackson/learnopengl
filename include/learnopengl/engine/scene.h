#pragma once

#include "learnopengl/engine/shader.h"
#include <learnopengl/engine/batch_render.h>
#include <learnopengl/engine/camera.h>
#include <learnopengl/engine/object.h>
#include <learnopengl/engine/render_context.h>
#include <learnopengl/utils/arraylist.h>
#include <learnopengl/utils/map.h>

#define MAX_SHADERS 20
#define STATIC_OBJ 0
#define DYNAMIC_OBJ 1

typedef struct {

  // list of dynamic  and static objects, referenced by Shader objects
  GameObject **staticObjects;
  GameObject **dynamicObjects;

  LightSource **lights;
  // keep a fixed size array for simplicity
  // realisitically more than 20 different shader programs is absurd
  // index 0 should be a default shader if object has no shader attached.

  RenderingContext renderingContext;
  // camera that the user controls via user input
  Camera *camera;

  BatchRenderer dynamicRenderer;
  BatchRenderer staticRenderer;

  uint maxLights;
  uint maxObjects;

  uint numLights;
  uint numObjectsDynamic;
  uint numObjectsStatic;

  uint numShaders;
  ShaderProgram **shaders;
  uint maxShaders;

  uint *textureIndicesDynamic;
  uint *textureIndicesStatic;
  uint numTextures;
  uint maxTextures;

  bool hasSkybox;

} Scene;

void initScene(Scene *scene, Camera *camera, uint maxShaders, uint maxObjects,
               uint maxLights);
void addObjectToScene(Scene *scene, GameObject *object, uint objectType);
void addLightToScene(Scene *scene, LightSource *light, uint objectType);
void setSkybox(Scene *scene, Texture *texture, Sampler *sampler,
               ShaderProgram *shaderProgram);

void preLoop(Scene *scene);

void renderLoop(Scene *scene);

void freeScene(Scene *scene);
