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

const uint SHADER_BUFFER_LIGHT_BINDING = 0;
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
  glGenBuffers(1, &(renderer->modelsSSBO));
  glBindBuffer(GL_SHADER_STORAGE_BUFFER, renderer->modelsSSBO);
  // set and bind this buffer
  glBufferData(GL_SHADER_STORAGE_BUFFER,
               sizeof(mat4) * renderer->maxObjectCount, NULL, GL_DYNAMIC_DRAW);

  // TODO: make this so i dont need to specify binding in shaders
  uint modelsIndex = glGetProgramResourceIndex(renderer->shaderProgram->id, GL_SHADER_STORAGE_BLOCK , "ModelTransforms");
  if (modelsIndex != GL_INVALID_INDEX){
    glShaderStorageBlockBinding(renderer->shaderProgram->id, modelsIndex, SHADER_BUFFER_MODELS_BIND);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, SHADER_BUFFER_MODELS_BIND,renderer->modelsSSBO );
  }

  // light source uniform
  glGenBuffers(1, &(renderer->lightSSBO));
  if (renderer->maxLights) {
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, renderer->lightSSBO);
    glBufferData(GL_SHADER_STORAGE_BUFFER,
                 sizeof(LightData) * renderer->maxLights,
                 NULL, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, SHADER_BUFFER_LIGHT_BINDING,
                     renderer->lightSSBO);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
  }

  glGenBuffers(1, &(renderer->materialUBO));
  glBindBuffer(GL_UNIFORM_BUFFER, renderer->materialUBO);
  glBufferData(GL_UNIFORM_BUFFER, sizeof(Material), NULL, GL_DYNAMIC_DRAW);

  uint materialBlockIndex =
      glGetUniformBlockIndex(renderer->shaderProgram->id, "Material");
  if (materialBlockIndex != GL_INVALID_INDEX){
    glUniformBlockBinding(renderer->shaderProgram->id, materialBlockIndex,
                          UNIFORM_MATERIAL_BINDING);
    glBindBufferBase(GL_UNIFORM_BUFFER, UNIFORM_MATERIAL_BINDING,
                   renderer->materialUBO);
  }

  // setup the ubo
  glGenBuffers(1, &(renderer->viewProjectionUBO));
  glBindBuffer(GL_UNIFORM_BUFFER, renderer->viewProjectionUBO);
  glBufferData(GL_UNIFORM_BUFFER, sizeof(mat4[2]), NULL, GL_DYNAMIC_DRAW);
  // get the block index in the shader program
  uint viewProjBlockIndex =
      glGetUniformBlockIndex(renderer->shaderProgram->id, "ViewProjection");
  if (viewProjBlockIndex != GL_INVALID_INDEX){
  // link block index to binding point
  glUniformBlockBinding(renderer->shaderProgram->id, viewProjBlockIndex,
                        UNIFORM_VIEW_PROJ_BINDING);
  // link ubo to binding point
  glBindBufferBase(GL_UNIFORM_BUFFER, UNIFORM_VIEW_PROJ_BINDING,
                   renderer->viewProjectionUBO);

  glBindBuffer(GL_UNIFORM_BUFFER, 0);

  }
  // unbind

  glBindVertexArray(0);
}

void initBatchRenderer(BatchRenderer *renderer, Camera *camera,
                       ShaderProgram *shaderProgram, Vertex *vertexBuffer,
                       uint *modelIndexBuffer, mat4 *modelMatBuffer,
                       uint maxVertices, uint maxObjects, uint maxLights) {
  renderer->maxVertexCount = maxVertices;
  renderer->maxObjectCount = maxObjects;
  renderer->maxLights = maxLights;
  renderer->camera = camera;
  renderer->vertexBuffer = vertexBuffer;
  renderer->modelMatBuffer = modelMatBuffer;
  renderer->objectIndexBuffer = modelIndexBuffer;
  renderer->numObjects = 0;
  renderer->numVertices = 0;
  renderer->numTextures = 0;
  renderer->numLights = 0;
  renderer->textures = NULL;
  renderer->lights = NULL;
  renderer->material = NULL;
  renderer->shaderProgram = shaderProgram;
  glGenVertexArrays(1, &(renderer->vao));
  setUpBuffers(renderer);
}

void setMaterial(BatchRenderer *renderer, Material *material) {
  renderer->material = material;
}
void setLights(BatchRenderer *renderer, LightSource *lights, uint numLights) {
  renderer->lights = lights;
  renderer->numLights = numLights;
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

  bindTexturesUniform(shaderProgram, renderer->textures, renderer->numTextures);
  // pass view and projection matrices to shader

  // start drawing contents in vao's buffers
  glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertex) * renderer->numVertices,
                  renderer->vertexBuffer);

  glBindBuffer(GL_ARRAY_BUFFER, renderer->objectIndexVBO);
  glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(uint) * renderer->numVertices,
                  renderer->objectIndexBuffer);

  glBindBuffer(GL_SHADER_STORAGE_BUFFER, renderer->modelsSSBO);
  glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0,
                  sizeof(mat4) * renderer->numObjects,
                  renderer->modelMatBuffer);

  if (renderer->lights) {
    glBindBuffer(GL_UNIFORM_BUFFER, renderer->lightSSBO);
    LightData lightData[renderer->numLights ];
    encodeLights(renderer->lights, renderer->numLights, viewProjBuffer[0],
                 lightData);
    glUniform1ui(glGetUniformLocation(shaderProgram->id, "numLights"), renderer->numLights);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(lightData), lightData);
  }

  if (renderer->material) {
    glBindBuffer(GL_UNIFORM_BUFFER, renderer->materialUBO);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(Material), renderer->material);
  }

  glBindBuffer(GL_UNIFORM_BUFFER, renderer->viewProjectionUBO);
  glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(viewProjBuffer),
                  viewProjBuffer[0]);

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
        renderer->numObjects;
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