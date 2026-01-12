#include "batch_render.h"
#include "camera.h"
#include "mesh.h"
#include "render.h"
#include "shader.h"
#include "textures.h"
#include <cglm/mat4.h>
#include <glad/glad.h>
#include <stdio.h>
#include <string.h>

void setUpBuffers(BatchRenderer *renderer) {
  // Initialize VAO
  glBindVertexArray(renderer->vao);

  // create VBO for accumulated vertex Information
  glGenBuffers(1, &(renderer->vbo));

  glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo);

  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * renderer->maxVertexCount,
               NULL, GL_DYNAMIC_DRAW);

  // create shader attributes
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);
  glEnableVertexAttribArray(0); // vertex position attr

  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)sizeof(vec3s));

  glEnableVertexAttribArray(1); // textur uv coords attr

  // create a new buffer for indexes into transformation matrices

  glGenBuffers(1, &(renderer->objectIndexVBO));
  glBindBuffer(GL_ARRAY_BUFFER, renderer->objectIndexVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(uint) * renderer->maxVertexCount,
               NULL, GL_DYNAMIC_DRAW);

  // create corresponding shader attribute

  glVertexAttribIPointer(2, 1, GL_INT, sizeof(int),
                        (void *)0);

  glEnableVertexAttribArray(2);

  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Generate Shader Storage Buffer object for pass trasnformation matrices
  glGenBuffers(1, &(renderer->ssbo));
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, renderer->ssbo);
  // set and bind this buffer
  glBufferData(GL_SHADER_STORAGE_BUFFER,
               sizeof(mat4) * renderer->maxObjectCount,
               NULL, GL_DYNAMIC_DRAW);

  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, renderer->ssbo);

  glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
  // unbind
  glBindVertexArray(0);
}

void initBatchRenderer(BatchRenderer *renderer, Camera *camera,
                       ShaderProgram *shaderProgram, Vertex *vertexBuffer,
                       uint *modelIndexBuffer, mat4 *modelMatBuffer,
                       uint maxVertices, uint maxObjects) {
  renderer->maxVertexCount = maxVertices;
  renderer->maxObjectCount = maxObjects;
  renderer->camera = camera;
  renderer->vertexBuffer = vertexBuffer;
  renderer->modelMatBuffer = modelMatBuffer;
  renderer->objectIndexBuffer = modelIndexBuffer;
  renderer->numObjects = 0;
  renderer->numVertices = 0;
  renderer->numTextures = 0;
  renderer->textures = NULL;
  renderer->shaderProgram = shaderProgram;
  glGenVertexArrays(1, &(renderer->vao));
  setUpBuffers(renderer);
}

void setTexutres(BatchRenderer *renderer, Texture *textures, uint numTextures) {
  renderer->textures = textures;
  renderer->numTextures = numTextures;
}

void flush(BatchRenderer *renderer) {
  if (renderer->numObjects == 0 || renderer->numVertices == 0)
    return;

  // get view matrix

  mat4 view;
  getViewMatrix(renderer->camera, view);

  // get projection matrix
  mat4 projection;
  getProjectionMatrix(renderer->camera, projection);

  mat4 vp;
  glm_mat4_mul(projection, view, vp);

  ShaderProgram *shaderProgram = renderer->shaderProgram;

  glBindVertexArray(renderer->vao);

  useShaderProgram(shaderProgram);

  bindTexturesUniform(shaderProgram, renderer->textures, renderer->numTextures);

  // pass view and projection matrices to shader

  glUniformMatrix4fv(glGetUniformLocation(shaderProgram->id, "vp"), 1,
                     GL_FALSE, vp[0]);

  // start drawing contents in vao's buffers
  glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex)*renderer->numVertices, renderer->vertexBuffer);

  glBindBuffer(GL_ARRAY_BUFFER, renderer->objectIndexVBO);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(uint)*renderer->numVertices, renderer->objectIndexBuffer);

  glBindBuffer(GL_SHADER_STORAGE_BUFFER, renderer->ssbo);
  glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(mat4)*renderer->numObjects, renderer->modelMatBuffer);

  // how to draw permanently
  glDrawArrays(GL_TRIANGLES, 0, renderer->numVertices);

  // cleanup after render
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER,0);
  glBindVertexArray(0);

  unbindShaderProgram();

  // empty queue

  renderer->numVertices = 0;
  renderer->numObjects = 0;
};

void renderObjectBatch(BatchRenderer *renderer, GameObject *object) {
if (!renderer->camera) {
    return;
  }

  int newNumVertices = renderer->numVertices + object->mesh->numVertices;
  int newNumObject = renderer->numObjects + 1;
  if (renderer->maxVertexCount <= newNumVertices ||
      renderer->maxObjectCount <= newNumObject) {
    flush(renderer);
    newNumObject = 1;
    newNumVertices = object->mesh->numVertices;
  }

  

  mat4 model;
  getModelMatrix(object, model);

  // add model matrix to ssbo buffer
  memcpy(renderer->modelMatBuffer + renderer->numObjects, model, sizeof(mat4));

  // add current index to all vertices in index vbo
  for (int i = 0; i < object->mesh->numVertices; i++){
    renderer->objectIndexBuffer[i + renderer->numVertices] =renderer->numObjects+1;
  }
        // renderer->numObjects;

  // add vertex data to vertex vbo
  memcpy(renderer->vertexBuffer + renderer->numVertices, object->mesh->vertices,
         object->mesh->numVertices * sizeof(Vertex));

  // get transform matrix for object

  renderer->numVertices = newNumVertices;
  renderer->numObjects = newNumObject;
  // maintain dictionary of shaders to buffer
  //
}

void destroyBatchRenderer(BatchRenderer *renderer) {}