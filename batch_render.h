#pragma once
#include "light.h"
#include "global_info.h"
#include "camera.h"
#include "mesh.h"
#include "object.h"
#include "shader.h"
#include <sys/types.h>

typedef struct {
  uint vao;
  uint vertexVbo;
  uint objectIndexVBO;

  TransformationMatrixManager* transformManager; 

  Vertex *vertexBuffer;
  uint *objectIndexBuffer;
  uint numVertices;
  uint maxVertexCount;
} BatchRenderer;

void initBatchRenderer(BatchRenderer *renderer, TransformationMatrixManager* transformMatrixManager,
                       uint maxVertices) ;

void flush(BatchRenderer *renderer);

void renderObjectBatch(BatchRenderer *renderer, GameObject *object) ;

void freeBatchRenderer(BatchRenderer *renderer) ;

