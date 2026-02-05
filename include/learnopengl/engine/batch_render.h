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


// keep track of shader for each draw call
// bind each shader
//


typedef struct {
  uint count;
  uint instanceCount;
  uint firstIndex;
  int baseVertex;
  uint baseInstance;
} DrawElementsIndirectCommand;

typedef ArrayList(DrawElementsIndirectCommand) ArrayList_drawcmd;
initArrayList(ArrayList_drawcmd,DrawElementsIndirectCommand, draw_cmd);
insertDataArrayList(ArrayList_drawcmd, DrawElementsIndirectCommand, draw_cmd);
extendArrayList(ArrayList_drawcmd, DrawElementsIndirectCommand, draw_cmd);
extendArrayByArray(ArrayList_drawcmd, DrawElementsIndirectCommand, draw_cmd);
insertArrayList(ArrayList_drawcmd, DrawElementsIndirectCommand, draw_cmd);
insertWithArray(ArrayList_drawcmd, DrawElementsIndirectCommand,draw_cmd)
appendArrayList(ArrayList_drawcmd, DrawElementsIndirectCommand, draw_cmd);
getAtArrayList(ArrayList_drawcmd, DrawElementsIndirectCommand, draw_cmd);
removeAtArrayList(ArrayList_drawcmd, DrawElementsIndirectCommand, draw_cmd);
popArrayList(ArrayList_drawcmd, DrawElementsIndirectCommand, draw_cmd);
destroyArrayList(ArrayList_drawcmd, DrawElementsIndirectCommand,draw_cmd);
reserveArrayList(ArrayList_drawcmd,DrawElementsIndirectCommand, draw_cmd);

// same shader and same mesh, you use instancing
//

typedef struct {

  // parallel arrays
  // keeps track of id of mesh linked to draw command
  uint* meshIds;
  DrawElementsIndirectCommand* commands;

  // buffers
  mat4** models;
  mat4** invModels;
  Material** materials;
  uint** textureIndices;

  uint* numModels;
  uint numDrawCommands;
  uint numTextures;
  // the shader to identify
  ShaderProgram* shaderProgram;

} RenderingData;

typedef struct{
  Mesh* mesh;
  uint elementIndex;
  uint vertexIndex;
 } MeshInfo;

typedef struct {
  uint vao;

  uint vertexVbo;
  uint vertexEbo;
  uint modelsVBO;
  uint invModelsVBO;
  uint drawCommandsBO;
  uint materialBO;
  uint textureIndexVBO;

  MeshInfo* meshes;


  Vertex *vertexBuffer;
  uint *elementsBuffer;
  RenderingData* renderingData;
  DrawElementsIndirectCommand* drawCommandBuffer;
  mat4* modelsBuffer;
  mat4* invModelsBuffer;
  Material*materialsBuffer;
  uint* textureIndicesBuffer;

  TextureManager textureManager;

  // whenver you draw do it for each draw call
  // whenever you send data, youconcatenate draw calls into the main buffers;

  // parallel arrays keep track of which draw cmd are given to which shader

  uint numVertices;
  uint numElements;
  uint numModels;
  uint numDrawCommands;
  uint numShaders;
  uint numMeshes;

  uint maxVertex;
  uint maxElements;
  uint maxModels;
  uint maxShaders;

} BatchRenderer;

void initBatchRenderer(BatchRenderer *renderer, uint maxVertices,
                       uint maxElements, uint maxModels, uint maxShaders) ;
/*Issue the draw call and reset the client-side buffers*/
void flush(BatchRenderer *renderer) ;

/*Send data to client-side buffers, automatically
flushing when limits are exceeded*/
void renderObjectBatch(BatchRenderer *renderer, GameObject *object) ;

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
void draw(BatchRenderer *renderer) ;

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
