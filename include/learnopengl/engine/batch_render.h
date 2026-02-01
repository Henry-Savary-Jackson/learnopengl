#pragma once
#include "learnopengl/utils/arraylist.h"
#include "learnopengl/utils/map.h"
#include <learnopengl/engine/camera.h>
#include <learnopengl/engine/global_info.h>
#include <learnopengl/engine/light.h>
#include <learnopengl/engine/mesh.h>
#include <learnopengl/engine/object.h>
#include <learnopengl/engine/shader.h>
#include <sys/types.h>

#define MAX_TEXTURES 48

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
  uint textureIndexVBO;

  Vertex *vertexBuffer;
  uint *elementsBuffer;
  mat4 * modelsBuffer;
  mat4 * invModelsBuffer;
  DrawElementsIndirectCommand *drawCommandsBuffer;
  Material* materialBuffer;
  uint* textureBuffer;

  uint numVertices;
  uint numElements;
  uint numModels;
  uint numTextures;
  uint numDrawCommands;

  uint maxVertex;
  uint maxElements;
  uint maxModels;
  uint maxTextures;

  HashMap_uint_uint meshIndices; // index into draw command buffer for any  mesh id
  ArrayList_i * meshTextures; // for each drawCommand to be GPU instanced, keep an arryalist of indices of their texture for each instance
  // that way we can use GPu instancing with different textures
  // whenver we flush, we concatenate all the texture indices for the different draw commands into on buffer that. This is then indexed by GlDrawId+glInstanceId to get the texture
  // inside the fragment shader.


} BatchRenderer;

void initBatchRenderer(BatchRenderer *renderer,
                       uint maxVertices, uint maxElements, uint maxModels);

/*Issue the draw call and reset the client-side buffers*/
void flush(BatchRenderer *renderer);

/*Send data to client-side buffers, automatically
flushing when limits are exceeded*/
void renderObjectBatch(BatchRenderer *renderer, GameObject *object);

/*Free all buffers and resources the renderer holds.*/
void freeBatchRenderer(BatchRenderer *renderer);

/*
Send all of the client-side buffers to the GPU.
*/
void sendDataToGPU(BatchRenderer* renderer);

/*
Use this, once all clinet side buffers have been transferred to
OpenGL buffers to issue the draw call to the GPU.
*/
void draw(BatchRenderer* renderer);

/* Use this to only update transformations matrices of objects */
void sendTransformationsToGPU(BatchRenderer* renderer);

/*
Use this to only copy an objectś data to the relevant buffers
If adding this exceeds capacity , CAPACITY_EXCEEDED will be returned.
*/
uint copyObjectToBuffer(BatchRenderer* renderer, GameObject* object);

/*
Use this to only copy an objectś transfomration matrix to the client-side buffer.
*/
void copyTransformationToBuffer(BatchRenderer* renderer);

#define BATCH_RENDER_NO_SPACE -1
#define BATCH_RENDER_OBJ_EXCEEDS -2
#define BATCH_RENDER_OK 0
