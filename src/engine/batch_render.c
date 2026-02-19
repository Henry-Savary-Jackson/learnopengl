#include "learnopengl/engine/object.h"
#include "learnopengl/engine/shader.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <assert.h>
#include <cglm/mat4.h>
#include <cglm/types.h>
#include <learnopengl/engine/batch_render.h>
#include <complex.h>
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
static const uint ATTR_LOCATION_MATERIAL_INDEX = 11;
static const uint ATTR_LOCATION_TEXTURE_INDEX = 12;

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

  glVertexAttribPointer(ATTR_LOCATION_MATERIAL_INDEX, 1, GL_FLOAT, GL_FALSE,
                        sizeof(uint), NULL);

  glEnableVertexAttribArray(ATTR_LOCATION_MATERIAL_INDEX);
  glVertexAttribDivisor(ATTR_LOCATION_MATERIAL_INDEX, 1);

  glGenBuffers(1, &(renderer->textureIndexVBO));
  glBindBuffer(GL_ARRAY_BUFFER, renderer->textureIndexVBO);
  glVertexAttribPointer(ATTR_LOCATION_TEXTURE_INDEX, 1, GL_FLOAT, GL_FALSE,
                        sizeof(uint), NULL);
  glEnableVertexAttribArray(ATTR_LOCATION_TEXTURE_INDEX);
  glVertexAttribDivisor(ATTR_LOCATION_TEXTURE_INDEX, 1);

  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glGenBuffers(1, &(renderer->drawCommandsBO));
  glBindBuffer(GL_DRAW_INDIRECT_BUFFER, renderer->drawCommandsBO);
  glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);

  // unbind
  glBindVertexArray(0);
}

void initBatchRenderer(BatchRenderer *renderer, uint maxVertices,
                       uint maxElements, uint maxModels, uint maxShaders) {
  renderer->maxVertex = maxVertices;
  renderer->maxElements = maxElements;
  renderer->maxModels = maxModels;
  renderer->numVertices = 0;
  renderer->numElements = 0;
  renderer->numModels = 0;
  renderer->numShaders = 0;
  renderer->numMeshes = 0;
  renderer->maxShaders = maxShaders;
  renderer->numDrawCommands = 0;

  renderer->materialsBuffer =
      malloc(sizeof(MaterialData) * renderer->maxModels);
  renderer->modelsBuffer = malloc(sizeof(mat4) * renderer->maxModels);
  renderer->invModelsBuffer = malloc(sizeof(mat4) * renderer->maxModels);
  renderer->textureIndicesBuffer = malloc(sizeof(uint) * renderer->maxModels);
  renderer->drawCommandBuffer =
      malloc(sizeof(DrawElementsIndirectCommand) * renderer->maxModels);

  renderer->vertexBuffer = malloc(sizeof(Vertex) * maxVertices);
  renderer->elementsBuffer = malloc(sizeof(uint) * maxVertices);

  renderer->renderingData =
      malloc(sizeof(RenderingData) * renderer->maxShaders);
  renderer->meshes = malloc(sizeof(MeshInfo **) * maxModels);
  glGenVertexArrays(1, &(renderer->vao));
  setUpBuffers(renderer);
}

void vertices_func(BatchRenderer *renderer) {
  glNamedBufferData(renderer->vertexVbo, sizeof(Vertex) * renderer->numVertices,
                    renderer->vertexBuffer, GL_STATIC_DRAW);
}

void initRenderData(BatchRenderer *renderer, ShaderProgram *shaderProgram) {
  // append to last index
  renderer->renderingData[renderer->numShaders] = (RenderingData){
      .shaderProgram = shaderProgram,
      .materialIndices = malloc(sizeof(MaterialData *) * renderer->maxModels),
      .models = malloc(sizeof(mat4 *) * renderer->maxModels),
      .invModels = malloc(sizeof(mat4 *) * renderer->maxModels),
      .numModels = malloc(sizeof(uint) * renderer->maxModels),
      .numDrawCommands = 0,
      .numTextures = 0,
      .commands =
          malloc(sizeof(DrawElementsIndirectCommand) * renderer->maxModels),
      .meshIds = malloc(sizeof(uint) * renderer->maxModels),
      .textureIndices = malloc(sizeof(uint *) * renderer->maxModels),
  };

  renderer->numShaders++;
}

// already prepped up buffers with the renderingData data
// keep track of offset for each shader
void drawRenderData(BatchRenderer *renderer, RenderingData *renderingData,
                    uint *offset) {
  // bind shader

  useShaderProgram(renderingData->shaderProgram);
  // draw call

  // draw elements
  // give offset into buffer on GPU
  glMultiDrawElementsIndirect(
      GL_TRIANGLES, GL_UNSIGNED_INT,
      (void *)(sizeof(DrawElementsIndirectCommand) * (*offset)),
      renderingData->numDrawCommands, 0);

  // increase offset into the draw commands for the GPU to render
  *offset += renderingData->numDrawCommands;
  // u
}

void sendRenderDataToBuffer(BatchRenderer *renderer,
                            RenderingData *renderingData, uint *totalModels,
                            uint *totalDrawCalls) {
  // for each drawCall
  // append the arrays in order
  for (int i = 0; i < renderingData->numDrawCommands; i++) {
    uint numModels = renderingData->numModels[i];

    renderingData->commands[i].baseInstance = *totalModels;

    memcpy(renderer->modelsBuffer + *totalModels, renderingData->models[i],
           sizeof(mat4) * numModels);
    memcpy(renderer->invModelsBuffer + *totalModels,
           renderingData->invModels[i], sizeof(mat4) * numModels);
    memcpy(renderer->materialsBuffer + *totalModels,
           renderingData->materialIndices[i], sizeof(mat4) * numModels);
    memcpy(renderer->textureIndicesBuffer + *totalModels,
           renderingData->textureIndices[i], sizeof(uint) * numModels);

    *totalModels += renderingData->numModels[i];
  }

  // copy draw cmds into buffer
  memcpy(renderer->drawCommandBuffer + *totalDrawCalls, renderingData->commands,
         sizeof(DrawElementsIndirectCommand) * renderingData->numDrawCommands);

  // increase offset
  *totalDrawCalls += renderingData->numDrawCommands;
}

void freeRenderData(RenderingData *renderingData) {
  for (int i = 0; i < renderingData->numDrawCommands; i++) {
    free(renderingData->models[i]);
    free(renderingData->invModels[i]);
    free(renderingData->materialIndices[i]);
    free(renderingData->textureIndices[i]);
  }
  free(renderingData->commands);
  free(renderingData->numModels);
  free(renderingData->models);
  free(renderingData->invModels);
  free(renderingData->textureIndices);
  free(renderingData->materialIndices);
  free(renderingData->meshIds);
}

void matrices(BatchRenderer *renderer) {
  glNamedBufferData(renderer->modelsVBO, sizeof(mat4) * renderer->numModels,
                    renderer->modelsBuffer, GL_STATIC_DRAW);

  glNamedBufferData(renderer->invModelsVBO, sizeof(mat4) * renderer->numModels,
                    renderer->invModelsBuffer, GL_STATIC_DRAW);
}
void materials(BatchRenderer *renderer) {
  glNamedBufferData(renderer->materialBO,
                    sizeof(MaterialData) * renderer->numModels,
                    renderer->materialsBuffer, GL_STATIC_DRAW);
}
void drawCommandBuffer(BatchRenderer *renderer) {
  glNamedBufferData(renderer->drawCommandsBO,
                    sizeof(DrawElementsIndirectCommand) *
                        renderer->numDrawCommands,
                    renderer->drawCommandBuffer, GL_STATIC_DRAW);
}

void indices(BatchRenderer *renderer) {
  glNamedBufferData(renderer->vertexEbo, sizeof(uint) * renderer->numElements,
                    renderer->elementsBuffer, GL_STATIC_DRAW);
}

void texture(BatchRenderer *renderer) {
  glNamedBufferData(renderer->textureIndexVBO,
                    sizeof(uint) * renderer->numModels,
                    renderer->textureIndicesBuffer, GL_STATIC_DRAW);
}

void sendTransformationsToGPU(BatchRenderer *renderer) {
  glBindVertexArray(renderer->vao);
  matrices(renderer);
}

void sendDataToGPU(BatchRenderer *renderer) {

  if (!renderer->numModels || !renderer->numVertices ||
      !renderer->numDrawCommands || !renderer->numShaders)
    return;

  uint totalModels = 0;
  uint totalDrawCalls = 0;
  for (int i = 0; i < renderer->numShaders; i++) {
    sendRenderDataToBuffer(renderer, renderer->renderingData + i, &totalModels,
                           &totalDrawCalls);
  }
  assert(totalModels == renderer->numModels);

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

uint copyObjectToRenderingData(BatchRenderer *renderer,
                               RenderingData *renderingData, MeshInfo *meshInfo,
                               GameObject *object, uint textureIndex) {
  uint meshIndex = 0;
  for (; meshIndex < renderingData->numDrawCommands; meshIndex++)
    if (renderingData->meshIds[meshIndex] == object->mesh->id)
      break;

  if (meshIndex == renderingData->numDrawCommands) {

    if (renderer->numDrawCommands > renderer->maxModels)
      return BATCH_RENDER_NO_SPACE;
    // adding a new draw command for the current mesh
    renderingData->meshIds[meshIndex] = object->mesh->id;

    // add
    renderingData->commands[meshIndex] = (DrawElementsIndirectCommand){
        .baseInstance = 0,
        .instanceCount = 0,
        .firstIndex = meshInfo->elementIndex,
        .baseVertex = meshInfo->vertexIndex,
        .count = object->mesh->numElements,
    };
    renderingData->invModels[meshIndex] =
        malloc(sizeof(mat4) * renderer->maxModels);
    renderingData->models[meshIndex] =
        malloc(sizeof(mat4) * renderer->maxModels);
    renderingData->materialIndices[meshIndex] =
        malloc(sizeof(uint) * renderer->maxModels);
    renderingData->textureIndices[meshIndex] =
        malloc(sizeof(uint) * renderer->maxModels);
    renderingData->numDrawCommands++;
    renderingData->numModels[meshIndex] = 0;
    renderer->numDrawCommands++;
  }
  renderingData->commands[meshIndex].instanceCount++;

  mat4 model;
  getModelMatrix(object, model);
  mat4 modelInverse;
  // not need to be precise, just very fast
  glm_mat4_inv_fast(model, modelInverse);
  glm_mat4_transpose(modelInverse);

  uint offset = renderingData->numModels[meshIndex];
  memcpy(renderingData->models[meshIndex] + offset, model, sizeof(mat4));
  memcpy(renderingData->invModels[meshIndex] + offset, modelInverse,
         sizeof(mat4));
  renderingData->materialIndices[meshIndex][offset] = object->material->id;

  // INDEX 0 is always some default texture
  renderingData->textureIndices[meshIndex][offset] =
      textureIndex >= 0 ? textureIndex : 0;

  renderingData->numModels[meshIndex]++;

  return BATCH_RENDER_OK;
}

uint addMeshesToRenderer(BatchRenderer *renderer, GameObject *object,
                         uint meshIndex) {
  if (renderer->maxVertex < object->mesh->numVertices ||
      renderer->maxElements < object->mesh->numElements) {
    // this one object cannot fit in the space given to render
    return BATCH_RENDER_OBJ_EXCEEDS;
  }
  int newNumVertices = renderer->numVertices + object->mesh->numVertices;
  int newNumElements = renderer->numElements + object->mesh->numElements;

  if (renderer->maxVertex <= newNumVertices ||
      renderer->maxElements <= newNumElements) {
    // there is no more space for the current object
    // calling function can choose to flush the renderer or simply stop
    // rendering
    return BATCH_RENDER_NO_SPACE;
  }

  renderer->meshes[meshIndex] =
      (MeshInfo){.mesh = object->mesh,
                 .elementIndex = renderer->numElements,
                 .vertexIndex = renderer->numVertices};

  memcpy(renderer->vertexBuffer + renderer->numVertices, object->mesh->vertices,
         object->mesh->numVertices * sizeof(Vertex));

  // add elemend index dat to EBO
  memcpy(renderer->elementsBuffer + renderer->numElements,
         object->mesh->elementOrder, object->mesh->numElements * sizeof(uint));

  renderer->numElements = newNumElements;
  renderer->numVertices = newNumVertices;
  renderer->numMeshes++;

  return BATCH_RENDER_OK;
}

static uint framecount = 0;
static float totalTimeDraw = 0;
static float totalTimeSend = 0;

uint copyObjectToBuffer(BatchRenderer *renderer, GameObject *object,
                        uint textureIndex) {

  if (renderer->maxModels <= renderer->numModels)
    return BATCH_RENDER_NO_SPACE;

  uint shaderIndex = 0;
  for (; shaderIndex < renderer->numShaders; shaderIndex++)
    if (renderer->renderingData[shaderIndex].shaderProgram->id ==
        object->shaderProgram->id)
      break;

  assert(shaderIndex < renderer->maxShaders);

  uint meshIndex = 0;
  for (; meshIndex < renderer->numMeshes; meshIndex++)
    if (renderer->meshes[meshIndex].mesh->id == object->mesh->id)
      break;

  assert(meshIndex < renderer->maxModels);
  if (meshIndex == renderer->numMeshes) {
    uint result = addMeshesToRenderer(renderer, object, meshIndex);
    if (result != BATCH_RENDER_OK)
      return result;
  }

  if (shaderIndex == renderer->numShaders) {
    if (shaderIndex > renderer->maxShaders)
      return BATCH_RENDER_NO_SPACE;
    // TODO: might it not be better that all objects are already grouped by
    // shadersa MAYBE YOU SHOULD INSTEAD HAVE a queue that will flush all once
    // all the shaders limits are reached or there are no more objects
    initRenderData(renderer, object->shaderProgram);
  }

  uint texIndex = object->numTextures > 0
                      ? glGetTextureSamplerHandleARB(object->textures->glID,
                                                     object->sampler->glID)
                      : 0;

  uint result = copyObjectToRenderingData(
      renderer, renderer->renderingData + shaderIndex,
      renderer->meshes + meshIndex, object, textureIndex);
  renderer->numModels++;
  return result;
}

void draw(BatchRenderer *renderer) {
  glBindVertexArray(renderer->vao);

  glBindBuffer(GL_DRAW_INDIRECT_BUFFER, renderer->drawCommandsBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer->vertexEbo);

  uint offset = 0;
  for (int i = 0; i < renderer->numShaders; i++) {
    drawRenderData(renderer, renderer->renderingData + i, &offset);
  }
  unbindShaderProgram();
}

void flush(BatchRenderer *renderer) {
  if (!renderer->numModels || !renderer->numVertices ||
      !renderer->numDrawCommands || !renderer->numShaders)
    return;


  sendDataToGPU(renderer);
  // framecount ++;
  // float start = glfwGetTime();
  // float endSend = glfwGetTime();
  draw(renderer);
  // float endDraw = glfwGetTime();

  // totalTimeDraw += endDraw - endSend;
  // totalTimeSend += endSend - start;

  // if (framecount % 20 == 0) {
  //   // printf("sending to GPU: %0.6f\n", totalTimeSend / 20);
  //   // printf("drawing: %0.6f\n", totalTimeDraw / 20);
  //   totalTimeDraw = 0;
  //   totalTimeSend = 0;
  // }

  renderer->numVertices = 0;
  renderer->numElements = 0;
  renderer->numMeshes = 0;
  renderer->numModels = 0;
  renderer->numShaders = 0;
  renderer->numDrawCommands = 0;
}

void renderObjectBatch(BatchRenderer *renderer, GameObject *object,
                       uint textureIndex) {
  int retCode = BATCH_RENDER_NO_SPACE;
  uint iters = 0;
  while (((retCode = copyObjectToBuffer(renderer, object, textureIndex)) ==
          BATCH_RENDER_NO_SPACE) &&
         iters < 20) {
    flush(renderer);
    iters++;
  }

  assert(retCode != BATCH_RENDER_OBJ_EXCEEDS);
}

void freeBatchRenderer(BatchRenderer *renderer) {

  glDeleteBuffers(1, &renderer->vertexVbo);
  glDeleteBuffers(1, &renderer->vertexEbo);
  glDeleteBuffers(1, &renderer->drawCommandsBO);
  glDeleteBuffers(1, &renderer->invModelsVBO);
  glDeleteBuffers(1, &renderer->modelsVBO);
  glDeleteBuffers(1, &renderer->textureIndexVBO);

  free(renderer->vertexBuffer);
  free(renderer->elementsBuffer);
  free(renderer->meshes);

  glDeleteVertexArrays(1, &renderer->vao);
}
// TODO: make allocations less error-prone
