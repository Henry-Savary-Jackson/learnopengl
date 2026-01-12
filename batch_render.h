#pragma once
#include "camera.h"
#include "mesh.h"
#include "object.h"
#include "shader.h"
#include <sys/types.h>

typedef struct {
  uint vao;
  uint vbo;
  uint objectIndexVBO;
  uint ssbo; // store all the model matrices to send to gpu

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
} BatchRenderer;

void initBatchRenderer(BatchRenderer *renderer, Camera *camera,
                       ShaderProgram *shaderProgram, void *vertexBuffer,
                       void *modelIndexBuffer, void *modelMatBuffer,
                       uint maxVertices);

void flush(BatchRenderer *renderer);
