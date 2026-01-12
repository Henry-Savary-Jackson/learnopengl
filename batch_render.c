#include "batch_render.h"
#include "camera.h"
#include "mesh.h"
#include "shader.h"
#include "textures.h"
#include <glad/glad.h>
#include <stdio.h>
#include <string.h>

void setUpBuffers(BatchRenderer *renderer) {
  // Initialize VAO
  glGenVertexArrays(1, &(renderer->vao));

  // create VBO for accumulated vertex Information
  glGenBuffers(1, &(renderer->vbo));

  glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo);

  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * renderer->maxVertexCount,
               renderer->vertexBuffer, GL_STATIC_DRAW);

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
               renderer->objectIndexBuffer, GL_STATIC_DRAW);

  // create corresponding shader attribute

  glVertexAttribPointer(2, 1, GL_UNSIGNED_INT, GL_FALSE, sizeof(uint),
                        (void *)0);

  glEnableVertexAttribArray(2);

  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Generate Shader Storage Buffer object for pass trasnformation matrices
  glGenBuffers(1, &(renderer->ssbo));
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, renderer->ssbo);
  // set and bind this buffer
  glBufferData(GL_SHADER_STORAGE_BUFFER,
               sizeof(mat4) * renderer->maxObjectCount,
               renderer->modelMatBuffer, GL_STATIC_READ);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, renderer->ssbo);

  glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

  // unbind
  glBindVertexArray(0);
}

void initBatchRenderer(BatchRenderer *renderer, Camera *camera,
                       ShaderProgram *shaderProgram, void *vertexBuffer,
                       void *modelIndexBuffer, void *modelMatBuffer,
                       uint maxVertices) {
  renderer->maxVertexCount = maxVertices;
  renderer->camera = camera;
  renderer->vertexBuffer = vertexBuffer;
  renderer->modelMatBuffer = modelMatBuffer;
  renderer->objectIndexBuffer = modelIndexBuffer;
  renderer->numObjects = 0;
  renderer->numVertices = 0;
  renderer->numTextures = 0;
  renderer->shaderProgram = shaderProgram;
  setUpBuffers(renderer);
}

void setTexutres(BatchRenderer* renderer, Texture* textures, uint numTextures){
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

  ShaderProgram *shaderProgram = renderer->shaderProgram;

  useShaderProgram(shaderProgram);

  bindTexturesUniform(shaderProgram, renderer->textures, renderer->numTextures);

  // pass view and projection matrices to shader

  glUniformMatrix4fv(glGetUniformLocation(shaderProgram->id, "view"), 1,
                     GL_FALSE, view[0]);

  glUniformMatrix4fv(glGetUniformLocation(shaderProgram->id, "projection"), 1,
                     GL_FALSE, projection[0]);

  // start drawing contents in vao's buffers
  glBindVertexArray(renderer->vao);
  glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo);

  glDrawArrays(GL_TRIANGLES, 0, renderer->numVertices);

  // cleanup after render
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glBindVertexArray(0);

  unbindShaderProgram(shaderProgram);

  // empty queue

  renderer->numVertices = 0;
  renderer->numObjects = 0;

};

void renderObjectBatch(BatchRenderer *renderer, GameObject *object) {

  int newNumVertices = renderer->numVertices += object->mesh->numVertices;
  int newNumObject = renderer->numObjects++;
  if (renderer->maxVertexCount <= newNumVertices ||
      renderer->maxObjectCount <= newNumObject) {
    flush(renderer);
  }

  if (!renderer->camera) {
    return;
  }

  mat4 model;
  getModelMatrix(object, model);

  // add model matrix to ssbo buffer
  memcpy(renderer->modelMatBuffer + renderer->numObjects, model, sizeof(mat4));

  // add current index to all vertices in index vbo
  memset(renderer->objectIndexBuffer + renderer->numVertices,
         renderer->numVertices, object->mesh->numVertices * sizeof(uint));

  // add vertex data to vertex vbo
  memcpy(renderer->vertexBuffer + renderer->numVertices, model, sizeof(Vertex));

  // get transform matrix for object

  renderer->numVertices = newNumVertices;
  renderer->numObjects = newNumObject;
  // maintain dictionary of shaders to buffer
  //
}

void destroyBatchRenderer(BatchRenderer *renderer) {}