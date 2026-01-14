#include "batch_render.h"
#include "camera.h"
#include "mesh.h"
#include "render.h"
#include "shader.h"
#include "textures.h"
#include <cglm/mat4.h>
#include <cglm/types.h>
#include <glad/glad.h>
#include <stdio.h>
#include <string.h>

const uint UNIFORM_LIGHT_BINDING = 0;
const uint UNIFORM_VIEW_PROJ_BINDING = 1;
const uint UNIFORM_MATERIAL_BINDING = 2;
const uint SHADER_BUFFER_MODELS_BIND = 3;

void setUpBuffers(BatchRenderer *renderer) {
  // Initialize VAO
  glBindVertexArray(renderer->vao);

  // create VBO for accumulated vertex Information
  glGenBuffers(1, &(renderer->vbo));

  glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo);

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

  glBindBuffer(GL_ARRAY_BUFFER, 0);

  // Generate Shader Storage Buffer object for pass trasnformation matrices
  glGenBuffers(1, &(renderer->ssbo));
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, renderer->ssbo);
  // set and bind this buffer
  glBufferData(GL_SHADER_STORAGE_BUFFER,
               sizeof(mat4) * renderer->maxObjectCount, NULL, GL_DYNAMIC_DRAW);

  // uint modelBlockIndex=glGetProgramResourceIndex(renderer->shaderProgram->id ,GL_SHADER_STORAGE_BLOCK, "ModelTransforms");
  // glUniformBlockBinding(renderer->shaderProgram->id, modelBlockIndex,
                        // SHADER_BUFFER_MODELS_BIND);
  glBindBufferBase(GL_SHADER_STORAGE_BUFFER, SHADER_BUFFER_MODELS_BIND, renderer->ssbo);

  glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

  // generate UBOs

  // light source uniform
  glGenBuffers(1, &(renderer->lightUBO));
  glBindBuffer(GL_UNIFORM_BUFFER, renderer->lightUBO);
  glBufferData(GL_UNIFORM_BUFFER, sizeof(LightSource), NULL, GL_DYNAMIC_DRAW);
  uint lightBlockIndex =
      glGetUniformBlockIndex(renderer->shaderProgram->id, "Light");
  glUniformBlockBinding(renderer->shaderProgram->id, lightBlockIndex,
                        UNIFORM_LIGHT_BINDING);
  glBindBufferBase(GL_UNIFORM_BUFFER, UNIFORM_LIGHT_BINDING,
                   renderer->lightUBO);

  glGenBuffers(1, &(renderer->materialUBO));
  glBindBuffer(GL_UNIFORM_BUFFER, renderer->materialUBO);
  glBufferData(GL_UNIFORM_BUFFER, sizeof(Material), NULL, GL_DYNAMIC_DRAW);
  uint materialBlockIndex =
      glGetUniformBlockIndex(renderer->shaderProgram->id, "Material");
  glUniformBlockBinding(renderer->shaderProgram->id, lightBlockIndex,
                        UNIFORM_MATERIAL_BINDING);
  glBindBufferBase(GL_UNIFORM_BUFFER, UNIFORM_MATERIAL_BINDING,
                   renderer->materialUBO);

  // setup the ubo
  glGenBuffers(1, &(renderer->viewProjectionUBO));
  glBindBuffer(GL_UNIFORM_BUFFER, renderer->viewProjectionUBO);
  glBufferData(GL_UNIFORM_BUFFER, sizeof(mat4[2]), NULL, GL_DYNAMIC_DRAW);
  // get the block index in the shader program
  uint viewProjBlockIndex =
      glGetUniformBlockIndex(renderer->shaderProgram->id, "ViewProjection");
  // link block index to binding point
  glUniformBlockBinding(renderer->shaderProgram->id, viewProjBlockIndex,
                        UNIFORM_VIEW_PROJ_BINDING);
  // link ubo to binding point
  glBindBufferBase(GL_UNIFORM_BUFFER, UNIFORM_VIEW_PROJ_BINDING,
                   renderer->viewProjectionUBO);

  // unbind
  glBindBuffer(GL_UNIFORM_BUFFER, 0);

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
  renderer->light = NULL;
  renderer->material = NULL;
  renderer->shaderProgram = shaderProgram;
  glGenVertexArrays(1, &(renderer->vao));
  setUpBuffers(renderer);
}

void setMaterial(BatchRenderer *renderer, Material *material) {
  renderer->material = material;
}
void setLight(BatchRenderer *renderer, LightSource *light) {
  renderer->light = light;
}

void setTexutres(BatchRenderer *renderer, Texture *textures, uint numTextures) {
  renderer->textures = textures;
  renderer->numTextures = numTextures;
}

void flush(BatchRenderer *renderer) {
  if (renderer->numObjects == 0 || renderer->numVertices == 0)
    return;


  // buffer to store view and projection matrices
  mat4 viewProjBuffer[2];
  // get view matrix
  getViewMatrix(renderer->camera, viewProjBuffer[0]);

  // get projection matrix
  getProjectionMatrix(renderer->camera, viewProjBuffer[1]);

  ShaderProgram *shaderProgram = renderer->shaderProgram;

  glBindVertexArray(renderer->vao);

  useShaderProgram(shaderProgram);


  vec3s lightPos = renderer->light->position;
  vec3s lightCol = renderer->light->colour;
  glUniform3f(glGetUniformLocation(shaderProgram->id, "lightPos"), lightPos.x, lightPos.y, lightPos.z );
  glUniform3f(glGetUniformLocation(shaderProgram->id, "lightColour"), lightCol.r, lightPos.g, lightCol.b );

  bindTexturesUniform(shaderProgram, renderer->textures, renderer->numTextures);

  // pass view and projection matrices to shader

  // start drawing contents in vao's buffers
  glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * renderer->numVertices,
                  renderer->vertexBuffer);

  glBindBuffer(GL_ARRAY_BUFFER, renderer->objectIndexVBO);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(uint) * renderer->numVertices,
                  renderer->objectIndexBuffer);

  glBindBuffer(GL_SHADER_STORAGE_BUFFER, renderer->ssbo);
  glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0,
                  sizeof(mat4) * renderer->numObjects,
                  renderer->modelMatBuffer);

  if (renderer->light) {
    glBindBuffer(GL_UNIFORM_BUFFER, renderer->lightUBO);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(LightSource), renderer->light);
  }

  if (renderer->material) {
    glBindBuffer(GL_UNIFORM_BUFFER, renderer->materialUBO);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(Material), renderer->material);
  }

  glBindBuffer(GL_UNIFORM_BUFFER, renderer->viewProjectionUBO);
  glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(viewProjBuffer),viewProjBuffer[0]);

  // how to draw permanently
  glDrawArrays(GL_TRIANGLES, 0, renderer->numVertices);

  // cleanup after render
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
  glBindBuffer(GL_UNIFORM_BUFFER, 0);
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
  memcpy(renderer->modelMatBuffer + renderer->numObjects, model, sizeof(mat4s));

  // add current index to all vertices in index vbo
  for (int i = 0; i < object->mesh->numVertices; i++) {
    renderer->objectIndexBuffer[i + renderer->numVertices] =
        renderer->numObjects ;
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