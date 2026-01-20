#include <learnopengl/engine/batch_render.h>
#include <learnopengl/engine/global_info.h>
#include <learnopengl/engine/mesh.h>
#include <learnopengl/engine/render.h>
#include <cglm/mat4.h>
#include <cglm/types.h>
#include <glad/glad.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const uint SHADER_BUFFER_LIGHT_BINDING = 0;
static const uint UNIFORM_VIEW_PROJ_BINDING = 1;
static const uint UNIFORM_MATERIAL_BINDING = 2;
static const uint SHADER_BUFFER_MODELS_BIND = 3;

static const uint ATTR_LOCATION_POS = 0;
static const uint ATTR_LOCATION_UV= 1;
static const uint ATTR_LOCATION_NORM = 2;

void setUpBuffers(BatchRenderer *renderer) {
  // Initialize VAO
  glBindVertexArray(renderer->vao);
  glGenBuffers(1, &(renderer->vertexEbo));
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer->vertexEbo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint)*renderer->maxElements, NULL, GL_STREAM_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  // create VBO for accumulated vertex Information
  glGenBuffers(1, &(renderer->vertexVbo));

  glBindBuffer(GL_ARRAY_BUFFER, renderer->vertexVbo);

  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * renderer->maxVertexCount, NULL,
               GL_STREAM_DRAW);

  // create shader attributes
  glVertexAttribPointer(ATTR_LOCATION_POS, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);
  glEnableVertexAttribArray(ATTR_LOCATION_POS); // vertex position attr

  glVertexAttribPointer(ATTR_LOCATION_UV, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)sizeof(vec3s));

  glEnableVertexAttribArray(ATTR_LOCATION_UV); // textur uv coords attr

  glVertexAttribPointer(ATTR_LOCATION_NORM, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)sizeof(vec3s) + sizeof(vec2s));

  glEnableVertexAttribArray(ATTR_LOCATION_NORM); // normal vectors attribute

  // create corresponding shader attribute

  glEnableVertexAttribArray(ATTR_LOCATION_NORM);
  // glVertexAttribDivisor(ATTR_LOCATION_POS, 0);
  // glVertexAttribDivisor(ATTR_LOCATION_NORM, 0);
  // glVertexAttribDivisor(ATTR_LOCATION_UV, 0);

  glGenBuffers(1, &(renderer->drawCommandsBO));
  glBindBuffer(GL_DRAW_INDIRECT_BUFFER,renderer->drawCommandsBO);
  glBufferData(GL_DRAW_INDIRECT_BUFFER, sizeof(DrawElementsIndirectCommand)*renderer->transformManager->maxModels, NULL, GL_STREAM_DRAW);
  glBindBuffer(GL_DRAW_INDIRECT_BUFFER,0);

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
  renderer->elementsBuffer = malloc(sizeof(uint)*maxVertices);
  renderer->drawCommandsBuffer = malloc(sizeof(DrawElementsIndirectCommand)*transformMatrixManager->maxModels);
  glGenVertexArrays(1, &(renderer->vao));
  setUpBuffers(renderer);
}

void flush(BatchRenderer *renderer) {
  if (renderer->transformManager->numModels == 0 || renderer->numVertices == 0 )
    return;

  printf("num objects: %d, num vertices:%d\n", renderer->transformManager->numModels, renderer->numVertices);

  glBindVertexArray(renderer->vao);
  TransformationMatrixManager* manager = renderer->transformManager;

  // send data in vbo buffers
  glBindBuffer(GL_ARRAY_BUFFER, renderer->vertexVbo);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * renderer->numVertices,
                  renderer->vertexBuffer);

  // flush data on transformation matrices
  uint numObjects = manager->numModels;
  flushTransformationManager(manager);
  numObjects = 1;

  // flush data on the draw commands
  glBindBuffer(GL_DRAW_INDIRECT_BUFFER, renderer->drawCommandsBO);
  glBufferSubData(GL_DRAW_INDIRECT_BUFFER, 0,sizeof(DrawElementsIndirectCommand)*numObjects, renderer->drawCommandsBuffer);


  // send the ebo info
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer->vertexEbo);
  glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, sizeof(uint) * renderer->numElements,
                  renderer->elementsBuffer);


  //draw elements
  glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, NULL,numObjects, 0);

  // unbind after render
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindBuffer(GL_DRAW_INDIRECT_BUFFER,0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
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
  memcpy(renderer->elementsBuffer + renderer->numElements, object->mesh->elementOrder,
         object->mesh->numElements * sizeof(uint));

  // add draw command to Buffer
  renderer->drawCommandsBuffer[0] =(DrawElementsIndirectCommand){
    .baseInstance=0,
    .instanceCount=newNumObject,
    .firstIndex=0,
    .baseVertex=0,
    .count=object->mesh->numElements,
  };

  // get transform matrix for object

  renderer->numVertices = object->mesh->numVertices;
  renderer->numElements = object->mesh->numElements;
  // maintain dictionary of shaders to buffer
  //
}

void freeBatchRenderer(BatchRenderer *renderer) {
  // free(renderer->vertexBuffer);
  free(renderer->elementsBuffer);
}
