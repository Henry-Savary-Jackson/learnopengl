#pragma once
#include <learnopengl/engine/camera.h>
#include <learnopengl/engine/global_info.h>
#include <learnopengl/engine/light.h>
#include <learnopengl/engine/mesh.h>
#include <learnopengl/engine/object.h>
#include <learnopengl/engine/shader.h>
#include <sys/types.h>

typedef struct {
  uint count;
  uint instanceCount;
  uint firstIndex;
  int baseVertex;
  uint baseInstance;
} DrawElementsIndirectCommand;

typedef struct {
  uint vao;

  uint vertexVbo;
  uint vertexEbo;
  uint modelsVBO;
  uint invModelsVBO;
  uint drawCommandsBO;
  uint materialBO;

  Vertex *vertexBuffer;
  uint *elementsBuffer;
  mat4 * modelsBuffer;
  mat4 * invModelsBuffer;
  DrawElementsIndirectCommand *drawCommandsBuffer;
  Material* materialBuffer;

  uint numElements;
  uint maxElements;
  uint numVertices;
  uint maxVertex;
  uint numModels;
  uint maxModels;
} BatchRenderer;

void initBatchRenderer(BatchRenderer *renderer,
                       uint maxVertices, uint maxElements, uint maxModels);

void flush(BatchRenderer *renderer);

void renderObjectBatch(BatchRenderer *renderer, GameObject *object);

void freeBatchRenderer(BatchRenderer *renderer);

void fillInData(BatchRenderer* renderer);
