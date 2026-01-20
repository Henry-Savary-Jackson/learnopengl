#pragma once
#include <learnopengl/engine/light.h>
#include <learnopengl/engine/global_info.h>
#include <learnopengl/engine/camera.h>
#include <learnopengl/engine/mesh.h>
#include <learnopengl/engine/object.h>
#include <learnopengl/engine/shader.h>
#include <sys/types.h>

typedef  struct {
    unsigned int  count;
    unsigned int  instanceCount;
    unsigned int  firstIndex;
    int           baseVertex;
    unsigned int  baseInstance;
} DrawElementsIndirectCommand;

typedef struct {
  uint vao;
  uint vertexVbo;
  uint objectIndexVBO;
  uint vertexEbo;

  TransformationMatrixManager* transformManager;

  uint drawCommandsBO;
  DrawElementsIndirectCommand* drawCommandsBuffer;

  Vertex *vertexBuffer;
  uint * elementsBuffer;

  uint *objectIndexBuffer;
  uint numElements;
  uint maxElements;
  uint numVertices;
  uint maxVertexCount;
} BatchRenderer;

void initBatchRenderer(BatchRenderer *renderer, TransformationMatrixManager* transformMatrixManager,
                       uint maxVertices) ;

void flush(BatchRenderer *renderer);

void renderObjectBatch(BatchRenderer *renderer, GameObject *object) ;

void freeBatchRenderer(BatchRenderer *renderer) ;
