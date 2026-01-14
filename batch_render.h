#pragma once
#include "camera.h"
#include "light.h"
#include "mesh.h"
#include "object.h"
#include "shader.h"
#include <sys/types.h>

typedef struct {
  uint vao;
  uint vbo;
  uint objectIndexVBO;
  uint ssbo; // store all the model matrices to send to gpu

  uint lightUBO;
  uint materialUBO;
  uint viewProjectionUBO;

  mat4 *modelMatBuffer;
  uint numObjects;
  uint maxObjectCount;

  Vertex *vertexBuffer;
  uint *objectIndexBuffer;
  uint numVertices;
  uint maxVertexCount;

  Texture *textures;
  uint numTextures;

  ShaderProgram *shaderProgram;
  Camera *camera;

  LightSource* light;
  Material* material;

} BatchRenderer;


void initBatchRenderer(BatchRenderer *renderer, Camera *camera,
                       ShaderProgram *shaderProgram, Vertex *vertexBuffer,
                       uint *modelIndexBuffer, mat4 *modelMatBuffer,
                       uint maxVertices, uint maxObjects) ;

void flush(BatchRenderer *renderer);

void renderObjectBatch(BatchRenderer *renderer, GameObject *object) ;

void setTexutres(BatchRenderer* renderer, Texture* textures, uint numTextures);

void setMaterial(BatchRenderer* renderer, Material* material);
void setLight(BatchRenderer* renderer, LightSource* light);
