#include <immintrin.h>
#include <learnopengl/engine/batch_render.h>
#include <learnopengl/engine/global_info.h>
#include <learnopengl/engine/mesh.h>
#include <learnopengl/engine/render.h>
#include <cglm/mat4.h>
#include <cglm/types.h>
#include <glad/glad.h>
#include <stdlib.h>
#include <string.h>
#include <xmmintrin.h>

static const uint SHADER_BUFFER_LIGHT_BINDING = 0;
static const uint UNIFORM_VIEW_PROJ_BINDING = 1;
static const uint UNIFORM_MATERIAL_BINDING = 2;
static const uint SHADER_BUFFER_MODELS_BIND = 3;

void setUpBuffers(BatchRenderer *renderer) {
  // Initialize VAO
  glBindVertexArray(renderer->vao);
  glGenBuffers(1, &(renderer->vertexEbo));
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer->vertexEbo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint)*renderer->maxElements, NULL, GL_DYNAMIC_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  // create VBO for accumulated vertex Information
  glGenBuffers(1, &(renderer->vertexVbo));

  glBindBuffer(GL_ARRAY_BUFFER, renderer->vertexVbo);

  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * renderer->maxVertexCount, NULL,
               GL_DYNAMIC_DRAW);

  // create shader attributes
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);
  glEnableVertexAttribArray(0); // vertex position attr

  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)sizeof(vec3s));

  glEnableVertexAttribArray(1); // textur uv coords attr

  glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)sizeof(vec3s) + sizeof(vec2s));

  glEnableVertexAttribArray(3); // normal vectors attribute

  // create a new buffer for indexes into transformation matrices

  glGenBuffers(1, &(renderer->objectIndexVBO));
  glBindBuffer(GL_ARRAY_BUFFER, renderer->objectIndexVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(uint) * renderer->maxVertexCount, NULL,
               GL_DYNAMIC_DRAW);

  // create corresponding shader attribute

  glVertexAttribIPointer(2, 1, GL_INT, sizeof(int), (void *)0);

  glEnableVertexAttribArray(2);

  // unbind
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

void initBatchRenderer(BatchRenderer *renderer, TransformationMatrixManager* transformMatrixManager,
                       uint maxVertices, uint maxElements) {
  renderer->maxVertexCount = maxVertices;
  renderer->numVertices = 0;
  renderer->numElements = 0;
  renderer->maxElements = maxElements ;
  renderer->transformManager = transformMatrixManager;
  renderer->vertexBuffer = malloc(sizeof(Vertex)*maxVertices);
  renderer->objectIndexBuffer = malloc(sizeof(uint)*maxVertices);
  glGenVertexArrays(1, &(renderer->vao));
  setUpBuffers(renderer);
}

void flush(BatchRenderer *renderer) {
  if (renderer->transformManager->numModels == 0 || renderer->numVertices == 0)
    return;

  glBindVertexArray(renderer->vao);

  // send data in vbo buffers
  glBindBuffer(GL_ARRAY_BUFFER, renderer->vertexVbo);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * renderer->numVertices,
                  renderer->vertexBuffer);

  glBindBuffer(GL_ARRAY_BUFFER, renderer->objectIndexVBO);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(uint) * renderer->numVertices,
                  renderer->objectIndexBuffer);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // flush data on transformation matrices
  flushTransformationManager(renderer->transformManager);

  // send the ebo info
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer->vertexEbo);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(uint) * renderer->numElements,
                  renderer->elementsBuffer);
  // how to draw permanently
  glDrawElements(GL_TRIANGLES,  renderer->numElements, GL_UNSIGNED_INT, NULL );

  // unbind after render
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  glGenBuffers(1, &(renderer->drawCommandsBO));
  glBindBuffer(GL_DRAW_INDIRECT_BUFFER,renderer->drawCommandsBO);
  glBufferData(GL_DRAW_INDIRECT_BUFFER, sizeof(DrawElementsIndirectCommand)*renderer->transformManager->maxModels, NULL, GL_DYNAMIC_DRAW);

  glBindBuffer(GL_DRAW_INDIRECT_BUFFER,0);
  glBindVertexArray(0);

  renderer->numVertices = 0;
  renderer->numElements = 0;


};

void renderObjectBatch(BatchRenderer *renderer, GameObject *object) {
  int newNumVertices = renderer->numVertices + object->mesh->numVertices;
  int newNumElements = renderer->numElements + object->mesh->numElements;

  TransformationMatrixManager* manager = renderer->transformManager;
  int currentObjectIndex = renderer->transformManager->numModels;
  int newNumObject = currentObjectIndex + 1;
  if (renderer->maxVertexCount <= newNumVertices ||
      manager->maxModels <= newNumObject || renderer->maxElements <= newNumElements) {
    flush(renderer);
    newNumVertices = object->mesh->numVertices;
    newNumElements = object->mesh->numElements;
  }

  // add current index to all vertices in index vbo

  mat4 transform;
  getModelMatrix(object, transform);
  addTransformationToManager(manager, transform);

  // add vertex data to vertex vbo
  memcpy(renderer->vertexBuffer + renderer->numVertices, object->mesh->vertices,
         object->mesh->numVertices * sizeof(Vertex));
  // add elemend index dat to EBO

  // add draw command to Buffer
  DrawElementsIndirectCommand  command ={
    .baseInstance=0,
    .firstIndex=renderer->numElements,
    .baseVertex=renderer->numVertices,
    .count=object->mesh->numElements,
    .instanceCount =1
  };
  renderer->drawCommandsBuffer[currentObjectIndex] = command;

  // get transform matrix for object

  renderer->numVertices = newNumVertices;
  renderer->numElements = newNumElements;
  // maintain dictionary of shaders to buffer
  //
}

void freeBatchRenderer(BatchRenderer *renderer) {
  free(renderer->vertexBuffer);
  free(renderer->objectIndexBuffer);
}
