#include "learnopengl/engine/object.h"
#include "learnopengl/utils/arraylist.h"
#include "learnopengl/utils/map.h"
#include <assert.h>
#include <cglm/mat4.h>
#include <cglm/types.h>
#include <glad/glad.h>
#include <learnopengl/engine/batch_render.h>
#include <learnopengl/engine/global_info.h>
#include <learnopengl/engine/mesh.h>
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
  // ! must bind before using namedBufferData to send data.
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer->vertexEbo);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  // create VBO for accumulated vertex Information
  glGenBuffers(1, &(renderer->vertexVbo));

  glBindBuffer(GL_ARRAY_BUFFER, renderer->vertexVbo);

  // create shader attributes
  glVertexAttribPointer(ATTR_LOCATION_POS, 3, GL_FLOAT, GL_FALSE,
                        sizeof(Vertex), (void *)offsetof(Vertex, pos));

  glEnableVertexAttribArray(ATTR_LOCATION_POS); // vertex position attr

  glVertexAttribPointer(ATTR_LOCATION_UV, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)offsetof(Vertex, texPos));

  glEnableVertexAttribArray(ATTR_LOCATION_UV); // textur uv coords attr

  glVertexAttribPointer(ATTR_LOCATION_NORM, 3, GL_FLOAT, GL_FALSE,
                        sizeof(Vertex), (void *)offsetof(Vertex, normal));

  glEnableVertexAttribArray(ATTR_LOCATION_NORM); // normal vectors attribute

  // create corresponding shader attribute

  size_t colSize = sizeof(vec4);
  size_t mat4Size = sizeof(mat4);
  glGenBuffers(1, &(renderer->modelsVBO));

  glBindBuffer(GL_ARRAY_BUFFER, renderer->modelsVBO);

  glVertexAttribPointer(ATTR_LOCATION_MODEL, 4, GL_FLOAT, GL_FALSE, mat4Size,
                        0);
  glVertexAttribPointer(ATTR_LOCATION_MODEL + 1, 4, GL_FLOAT, GL_FALSE,
                        mat4Size, (void *)colSize);
  glVertexAttribPointer(ATTR_LOCATION_MODEL + 2, 4, GL_FLOAT, GL_FALSE,
                        mat4Size, (void *)(2 * colSize));
  glVertexAttribPointer(ATTR_LOCATION_MODEL + 3, 4, GL_FLOAT, GL_FALSE,
                        mat4Size, (void *)(3 * colSize));

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

  glVertexAttribPointer(ATTR_LOCATION_INV_MODEL, 4, GL_FLOAT, GL_FALSE,
                        mat4Size, 0);
  glVertexAttribPointer(ATTR_LOCATION_INV_MODEL + 1, 4, GL_FLOAT, GL_FALSE,
                        mat4Size, (void *)colSize);
  glVertexAttribPointer(ATTR_LOCATION_INV_MODEL + 2, 4, GL_FLOAT, GL_FALSE,
                        mat4Size, (void *)(2 * colSize));
  glVertexAttribPointer(ATTR_LOCATION_INV_MODEL + 3, 4, GL_FLOAT, GL_FALSE,
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
  renderer->numDrawCommands = 0;
  renderer->vertexBuffer = malloc(sizeof(Vertex) * maxVertices);
  renderer->elementsBuffer = malloc(sizeof(uint) * maxVertices);
  renderer->drawCommandsBuffer =
      malloc(sizeof(DrawElementsIndirectCommand) * maxModels);
  renderer->modelsBuffer = malloc(sizeof(mat4) * maxModels);
  renderer->invModelsBuffer = malloc(sizeof(mat4) * maxModels);
  renderer->materialBuffer = malloc(sizeof(Material) * maxModels);
  renderer->textureBuffer = malloc(sizeof(uint) * maxModels);

  initHash_uint_uint(&renderer->meshIndices, 6);

  glGenVertexArrays(1, &(renderer->vao));
  setUpBuffers(renderer);
}

void vertices_func(BatchRenderer *renderer) {

  glNamedBufferData(renderer->vertexVbo, sizeof(Vertex) * renderer->numVertices,
                    renderer->vertexBuffer, GL_STATIC_DRAW);
}

void matrices(BatchRenderer *renderer) {

  glNamedBufferData(renderer->modelsVBO, sizeof(mat4) * renderer->numModels,
                    renderer->modelsBuffer, GL_STATIC_DRAW);

  glNamedBufferData(renderer->invModelsVBO, sizeof(mat4) * renderer->numModels,
                    renderer->invModelsBuffer, GL_STATIC_DRAW);
}
void materials(BatchRenderer *renderer) {

  glNamedBufferData(renderer->materialBO,
                    sizeof(Material) * renderer->numModels,
                    renderer->materialBuffer, GL_STATIC_DRAW);
}
void drawCommandBuffer(BatchRenderer *renderer) {

  glNamedBufferData(renderer->drawCommandsBO,
                    sizeof(DrawElementsIndirectCommand) *
                        renderer->numDrawCommands,
                    renderer->drawCommandsBuffer, GL_STATIC_DRAW);
}

void indices(BatchRenderer *renderer) {
  glNamedBufferData(renderer->vertexEbo, sizeof(uint) * renderer->numElements,
                    renderer->elementsBuffer, GL_STATIC_DRAW);
}

void texture(BatchRenderer *renderer) {
  uint offsetTextureBuf = 0;
  for (int i = 0; i < renderer->numDrawCommands; i++) {
    ArrayList_i *list = renderer->meshTextures + i;
    memcpy(renderer->textureBuffer + offsetTextureBuf, list->ptr,
           sizeof(uint) * list->length);
    offsetTextureBuf += list->length;
    destroyArrayList_i(list);
  }
  glNamedBufferData(renderer->textureIndexVBO, sizeof(uint) * offsetTextureBuf,
                    renderer->textureBuffer, GL_STATIC_DRAW);
}

void sendTransformationsToGPU(BatchRenderer *renderer) {
  glBindVertexArray(renderer->vao);
  matrices(renderer);
}

void sendDataToGPU(BatchRenderer *renderer) {
  glBindVertexArray(renderer->vao);

  // send data in vbo buffers
  vertices_func(renderer);

  // flush data on transformation matrices
  matrices(renderer);

  // send data on material properties (reaction to light)
  materials(renderer);

  // send the ebo info
  indices(renderer);

  // flush data on texture sampler indices
  texture(renderer);

  // flush data on the draw commands
  drawCommandBuffer(renderer);
}

uint copyObjectToBuffer(BatchRenderer *renderer, GameObject* object) {

  int newNumVertices = renderer->numVertices + object->mesh->numVertices;
  int newNumElements = renderer->numElements + object->mesh->numElements;

  int currentObjectIndex = renderer->numModels;

  int newNumModels = currentObjectIndex + 1;

  if (renderer->maxVertex < object->mesh->numVertices || renderer->maxElements < object->mesh->numElements){
    return BATCH_RENDER_OBJ_EXCEEDS;
  }
  if (renderer->maxVertex <= newNumVertices ||
      renderer->maxModels <= newNumModels ||
      renderer->maxElements <= newNumElements ||
      renderer->numDrawCommands >= renderer->maxModels) {

      return BATCH_RENDER_NO_SPACE;
  }
    uint drawCommandIndex;
  uint result = get_uint_uint(&renderer->meshIndices, object->mesh->id,
                              &drawCommandIndex);
  if (result != RESULT_SUCCESS) {
    // we already have the mesh data, so just instance it.

    drawCommandIndex = renderer->numDrawCommands;
    renderer->numDrawCommands++;

    put_uint_uint(&renderer->meshIndices, object->mesh->id, drawCommandIndex);

    // init new command buffer
    renderer->drawCommandsBuffer[drawCommandIndex] =
        (DrawElementsIndirectCommand){
            .baseInstance = 0,
            .instanceCount = 0,
            .firstIndex = 0,
            .baseVertex = 0,
            .count = object->mesh->numElements,
        };
    initArrayList_i(&renderer->meshTextures[drawCommandIndex], 50, 10);

    // add vertex data to vertex vbo
    memcpy(renderer->vertexBuffer + renderer->numVertices,
           object->mesh->vertices, object->mesh->numVertices * sizeof(Vertex));

    // add elemend index dat to EBO
    memcpy(renderer->elementsBuffer + renderer->numElements,
           object->mesh->elementOrder,
           object->mesh->numElements * sizeof(uint));

    renderer->numVertices = newNumVertices;
    renderer->numElements = newNumElements;
  }

  mat4 model;
  getModelMatrix(object, model);
  mat4 modelInverse;
  // not need to be precise, just very fast
  glm_mat4_inv_fast(model, modelInverse);
  glm_mat4_transpose(modelInverse);

  memcpy(renderer->modelsBuffer + renderer->numModels, model, sizeof(mat4));
  memcpy(renderer->invModelsBuffer + renderer->numModels, modelInverse,
         sizeof(mat4));

  // add current index to all vertices in index vbo
  memcpy(renderer->materialBuffer + renderer->numModels, object->material,
         sizeof(Material));

  // update count of element to drawCommandBuffer
  renderer->drawCommandsBuffer[drawCommandIndex].instanceCount++;
  // add texture id to count
  if (object->numTextures > 0) {
    glActiveTexture(GL_TEXTURE0 + renderer->numTextures);
    glBindTexture(GL_TEXTURE_2D, object->textures[0].id);
    renderer->numTextures++;
    append_i(&renderer->meshTextures[drawCommandIndex], object->textures[0].id);
    // TODO: GET A DEFAULT TEXTURE
  }

  renderer->numModels = newNumModels;

  return 0;
}

void draw(BatchRenderer *renderer) {
  glBindBuffer(GL_DRAW_INDIRECT_BUFFER, renderer->drawCommandsBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer->vertexEbo);
  // draw elements
  glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, NULL,
                              renderer->numDrawCommands, 0);
}

void flush(BatchRenderer *renderer) {
  if (!renderer->numModels || !renderer->numVertices ||
      !renderer->numDrawCommands)
    return;

  draw(renderer);

  renderer->numVertices = 0;
  renderer->numElements = 0;
  renderer->numModels = 0;
  renderer->numDrawCommands = 0;
};

void renderObjectBatch(BatchRenderer *renderer, GameObject *object) {
  // int newNumVertices = renderer->numVertices + object->mesh->numVertices;
  // int newNumElements = renderer->numElements + object->mesh->numElements;

  // int currentObjectIndex = renderer->numModels;

  // int newNumModels = currentObjectIndex + 1;

  uint retCode;
  do {
    retCode = copyObjectToBuffer(renderer, object);
    if (retCode == BATCH_RENDER_NO_SPACE){
      flush(renderer);
      initHash_uint_uint(&renderer->meshIndices, 4);
    }

  }while (retCode == BATCH_RENDER_NO_SPACE);

  assert(retCode != BATCH_RENDER_OBJ_EXCEEDS);
}

void freeBatchRenderer(BatchRenderer *renderer) {

  for (int i = 0; i < renderer->numDrawCommands; i++) {
    destroyArrayList_i(&renderer->meshTextures[i]);
  }
  freeMap_uint_uint(&renderer->meshIndices);

  glDeleteBuffers(1, &renderer->vertexVbo);
  glDeleteBuffers(1, &renderer->vertexEbo);
  glDeleteBuffers(1, &renderer->drawCommandsBO);
  glDeleteBuffers(1, &renderer->invModelsVBO);
  glDeleteBuffers(1, &renderer->modelsVBO);
  glDeleteBuffers(1, &renderer->textureIndexVBO);

  free(renderer->vertexBuffer);
  free(renderer->elementsBuffer);
  free(renderer->drawCommandsBuffer);
  free(renderer->invModelsBuffer);
  free(renderer->modelsBuffer);
  free(renderer->textureBuffer);

  glDeleteVertexArrays(1, &renderer->vao);
}
// TODO: make allocations less error-prone
