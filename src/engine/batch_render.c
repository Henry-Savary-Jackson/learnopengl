#include "learnopengl/engine/object.h"
#include <cglm/mat4.h>
#include <cglm/types.h>
#include <glad/glad.h>
#include <learnopengl/engine/batch_render.h>
#include <learnopengl/engine/global_info.h>
#include <learnopengl/engine/mesh.h>
#include <learnopengl/engine/render.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const uint SHADER_BUFFER_LIGHT_BINDING = 0;
static const uint UNIFORM_VIEW_PROJ_BINDING = 1;
static const uint UNIFORM_MATERIAL_BINDING = 2;
static const uint SHADER_BUFFER_MODELS_BIND = 3;

static const uint ATTR_LOCATION_POS = 0;
static const uint ATTR_LOCATION_UV = 1;
static const uint ATTR_LOCATION_NORM = 2;
static const uint ATTR_LOCATION_MODEL = 3;
static const uint ATTR_LOCATION_INV_MODEL = 7;
static const uint ATTR_LOCATION_MATERIAL_COLOUR = 11;
static const uint ATTR_LOCATION_MATERIAL_COEFF = 12;

void setUpBuffers(BatchRenderer *renderer) {
  // Initialize VAO
  glBindVertexArray(renderer->vao);

  glGenBuffers(1, &(renderer->vertexEbo));
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer->vertexEbo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * renderer->maxElements,
               NULL, GL_STREAM_DRAW);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  // create VBO for accumulated vertex Information
  glGenBuffers(1, &(renderer->vertexVbo));

  glBindBuffer(GL_ARRAY_BUFFER, renderer->vertexVbo);

  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * renderer->maxVertex, NULL,
               GL_STREAM_DRAW);

  // create shader attributes
  glVertexAttribPointer(ATTR_LOCATION_POS, 3, GL_FLOAT, GL_FALSE,
                        sizeof(Vertex), (void *)offsetof(Vertex, pos));

  glEnableVertexAttribArray(ATTR_LOCATION_POS); // vertex position attr

  glVertexAttribPointer(ATTR_LOCATION_UV, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)offsetof(Vertex, texPos));

  glEnableVertexAttribArray(ATTR_LOCATION_UV); // textur uv coords attr

  glVertexAttribPointer(ATTR_LOCATION_NORM, 3, GL_FLOAT, GL_FALSE,
                        sizeof(Vertex), (void *)offsetof(Vertex,normal ));

  glEnableVertexAttribArray(ATTR_LOCATION_NORM); // normal vectors attribute

  // create corresponding shader attribute

  size_t colSize = sizeof(vec4);
  size_t mat4Size = sizeof(mat4);
  glGenBuffers(1, &(renderer->modelsVBO));

  glBindBuffer(GL_ARRAY_BUFFER, renderer->modelsVBO);
  glBufferData(GL_ARRAY_BUFFER, mat4Size * renderer->maxModels, NULL,
               GL_STREAM_DRAW);

  glVertexAttribPointer(ATTR_LOCATION_MODEL, 4, GL_FLOAT, GL_FALSE, mat4Size,
                        0);
  glVertexAttribPointer(ATTR_LOCATION_MODEL+1, 4, GL_FLOAT, GL_FALSE, mat4Size,
                        (void *)colSize);
  glVertexAttribPointer(ATTR_LOCATION_MODEL+2, 4, GL_FLOAT, GL_FALSE, mat4Size,
                        (void *)(2 * colSize));
  glVertexAttribPointer(ATTR_LOCATION_MODEL+3, 4, GL_FLOAT, GL_FALSE, mat4Size,
                        (void *)(3 * colSize));

  glEnableVertexAttribArray(ATTR_LOCATION_MODEL);
  glVertexAttribDivisor(ATTR_LOCATION_MODEL, 1);
  glEnableVertexAttribArray(ATTR_LOCATION_MODEL + 1);
  glVertexAttribDivisor(ATTR_LOCATION_MODEL + 1, 1);
  glEnableVertexAttribArray(ATTR_LOCATION_MODEL + 2);
  glVertexAttribDivisor(ATTR_LOCATION_MODEL + 2, 1);
  glEnableVertexAttribArray(ATTR_LOCATION_MODEL + 3);
  glVertexAttribDivisor(ATTR_LOCATION_MODEL + 3, 1);

  glGenBuffers(1, &(renderer->invModelsVBO));

  glBindBuffer(GL_ARRAY_BUFFER, renderer->invModelsVBO);
  glBufferData(GL_ARRAY_BUFFER, mat4Size * renderer->maxModels, NULL,
               GL_STREAM_DRAW);

  glVertexAttribPointer(ATTR_LOCATION_INV_MODEL, 4, GL_FLOAT, GL_FALSE,
                        mat4Size, 0);
  glVertexAttribPointer(ATTR_LOCATION_INV_MODEL+1, 4, GL_FLOAT, GL_FALSE,
                        mat4Size, (void *)colSize);
  glVertexAttribPointer(ATTR_LOCATION_INV_MODEL+2, 4, GL_FLOAT, GL_FALSE,
                        mat4Size, (void *)(2 * colSize));
  glVertexAttribPointer(ATTR_LOCATION_INV_MODEL+3, 4, GL_FLOAT, GL_FALSE,
                        mat4Size, (void *)(3 * colSize));

  glEnableVertexAttribArray(ATTR_LOCATION_INV_MODEL);
  glVertexAttribDivisor(ATTR_LOCATION_INV_MODEL, 1);
  glEnableVertexAttribArray(ATTR_LOCATION_INV_MODEL + 1);
  glVertexAttribDivisor(ATTR_LOCATION_INV_MODEL + 1, 1);
  glEnableVertexAttribArray(ATTR_LOCATION_INV_MODEL + 2);
  glVertexAttribDivisor(ATTR_LOCATION_INV_MODEL + 2, 1);
  glEnableVertexAttribArray(ATTR_LOCATION_INV_MODEL + 3);

  glGenBuffers(1, &(renderer->materialBO));
  glBindBuffer(GL_ARRAY_BUFFER, renderer->materialBO);
  glBufferData(GL_ARRAY_BUFFER, renderer->maxModels * sizeof(Material), NULL,
               GL_STREAM_DRAW);

  glVertexAttribPointer(ATTR_LOCATION_MATERIAL_COLOUR, 3, GL_FLOAT, GL_FALSE,
                        sizeof(Material), (void *)offsetof(Material, colour));

  glEnableVertexAttribArray(ATTR_LOCATION_MATERIAL_COLOUR);
  glVertexAttribDivisor(ATTR_LOCATION_MATERIAL_COLOUR, 1);

  glVertexAttribPointer(ATTR_LOCATION_MATERIAL_COEFF, 3, GL_FLOAT, GL_FALSE,
                        sizeof(Material),
                        (void *)offsetof(Material, ambientCoeffient));

  glEnableVertexAttribArray(ATTR_LOCATION_MATERIAL_COEFF);
  glVertexAttribDivisor(ATTR_LOCATION_MATERIAL_COEFF, 1);

  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glGenBuffers(1, &(renderer->drawCommandsBO));
  glBindBuffer(GL_DRAW_INDIRECT_BUFFER, renderer->drawCommandsBO);
  glBufferData(GL_DRAW_INDIRECT_BUFFER,
               sizeof(DrawElementsIndirectCommand) * renderer->maxModels, NULL,
               GL_STREAM_DRAW);
  glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);

  // unbind
  glBindVertexArray(0);
}

void initBatchRenderer(BatchRenderer *renderer, uint maxVertices,
                       uint maxElements, uint maxModels) {
  renderer->maxVertex = maxVertices;
  renderer->maxElements = maxElements;
  renderer->maxModels = maxModels;
  renderer->numVertices = 0;
  renderer->numElements = 0;
  renderer->numModels = 0;
  renderer->vertexBuffer = malloc(sizeof(Vertex) * maxVertices);
  renderer->elementsBuffer = malloc(sizeof(uint) * maxVertices);
  renderer->drawCommandsBuffer =
      malloc(sizeof(DrawElementsIndirectCommand) * maxModels);
  renderer->modelsBuffer = malloc(sizeof(mat4) * maxModels);
  renderer->invModelsBuffer = malloc(sizeof(mat4) * maxModels);
  renderer->materialBuffer = malloc(sizeof(Material) * maxModels);
  glGenVertexArrays(1, &(renderer->vao));
  setUpBuffers(renderer);
}

void flush(BatchRenderer *renderer) {
  if (!renderer->numModels || !renderer->numVertices )
    return;

  printf("num objects: %d, num vertices:%d\n", renderer->numModels,
         renderer->numVertices);

  glBindVertexArray(renderer->vao);

  // send data in vbo buffers
  glBindBuffer(GL_ARRAY_BUFFER, renderer->vertexVbo);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * renderer->numVertices,
                  renderer->vertexBuffer);

  glBindBuffer(GL_ARRAY_BUFFER, renderer->materialBO);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Material) * renderer->numModels,
                  renderer->materialBuffer);

  glBindBuffer(GL_ARRAY_BUFFER, renderer->modelsVBO);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(mat4) * renderer->numModels,
                  renderer->modelsBuffer);

  glBindBuffer(GL_ARRAY_BUFFER, renderer->invModelsVBO);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(mat4) * renderer->numModels,
                  renderer->invModelsBuffer);



  // flush data on transformation matrices
  uint numObjects = 1;

  // flush data on the draw commands
  glBindBuffer(GL_DRAW_INDIRECT_BUFFER, renderer->drawCommandsBO);
  glBufferSubData(GL_DRAW_INDIRECT_BUFFER, 0,
                  sizeof(DrawElementsIndirectCommand) * numObjects,
                  renderer->drawCommandsBuffer);

  // send the ebo info
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer->vertexEbo);
  glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0,
                  sizeof(uint) * renderer->numElements,
                  renderer->elementsBuffer);

  // draw elements
  glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, NULL,
  1, 0);


  // unbind after render
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  renderer->numVertices = 0;
  renderer->numElements = 0;
  renderer->numModels = 0;
};

void renderObjectBatch(BatchRenderer *renderer, GameObject *object) {
  int newNumVertices = renderer->numVertices + object->mesh->numVertices;
  int newNumElements = renderer->numElements + object->mesh->numElements;

  int currentObjectIndex = renderer->numModels;

  int newNumModels = currentObjectIndex + 1;

  if (renderer->maxVertex <= newNumVertices ||
      renderer->maxModels <= newNumModels ||
      renderer->maxElements <= newNumElements) {
    flush(renderer);

    newNumVertices = object->mesh->numVertices;
    newNumModels = 1;
    newNumElements = object->mesh->numElements;
  }


  mat4 model;
  getModelMatrix(object, model);
  mat4 modelInverse;
  glm_mat4_inv_fast(model, modelInverse);
  glm_mat4_transpose(modelInverse);

  // not need to be precise, just very fast
  memcpy(renderer->modelsBuffer + renderer->numModels, model, sizeof(mat4));
  memcpy(renderer->invModelsBuffer + renderer->numModels, modelInverse,
         sizeof(mat4));


  // add current index to all vertices in index vbo
  memcpy(renderer->materialBuffer + renderer->numModels, object->material,
         sizeof(Material));

  // add vertex data to vertex vbo
  memcpy(renderer->vertexBuffer + renderer->numVertices, object->mesh->vertices,
         object->mesh->numVertices * sizeof(Vertex));

  // add elemend index dat to EBO
  memcpy(renderer->elementsBuffer + renderer->numElements,
         object->mesh->elementOrder, object->mesh->numElements * sizeof(uint));

  // add draw command to Buffer
  renderer->drawCommandsBuffer[0] = (DrawElementsIndirectCommand){
      .baseInstance = 0,
      .instanceCount = newNumModels,
      .firstIndex = 0,
      .baseVertex = 0,
      .count = object->mesh->numElements,
  };

  // get transform matrix for object

  renderer->numVertices = object->mesh->numVertices;
  renderer->numElements = object->mesh->numElements;
  renderer->numModels = newNumModels;
  // maintain dictionary of shaders to buffer
  //
}

void freeBatchRenderer(BatchRenderer *renderer) {
  free(renderer->vertexBuffer);
  free(renderer->elementsBuffer);
  free(renderer->drawCommandsBuffer);
  free(renderer->invModelsBuffer);
  free(renderer->modelsBuffer);
}
