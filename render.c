#include "render.h"
#include "camera.h"
#include "mesh.h"
#include "shader.h"
#include "textures.h"
#include <cglm/mat4.h>
#include <cglm/struct/mat4.h>
#include <glad/glad.h>
#include <string.h>

void initRenderer(Renderer *renderer, Camera *camera) {
  renderer->camera = camera;
  glGenVertexArrays(1, &(renderer->vao));
}



void getModelViewProj(Camera *camera, GameObject *object, mat4 dest) {
  mat4 model ;
  getModelMatrix(object, model);

  mat4 view ;
  getViewMatrix(camera, view);

  mat4 projection;
  getProjectionMatrix(camera, projection);

  mat4 view_model;
  glm_mat4_mul(view, model,  view_model);
  glm_mat4_mul(projection,view_model, dest);
}

void setBuffersRenderer(Renderer *renderer, GameObject *object) {

  glBindVertexArray(renderer->vao);
  glGenBuffers(1, &(renderer->ebo));

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer->ebo);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER,
               sizeof(uint) * object->mesh->numElements,
               object->mesh->elementOrder, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  glGenBuffers(1, &(renderer->vbo));

  glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo);

  glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * object->mesh->numVertices,
               object->mesh->vertices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);
  glEnableVertexAttribArray(0); // vertex pos

  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                        (void *)sizeof(vec3s));
  glEnableVertexAttribArray(1); // texture coords

  // unbind
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

void renderObject(Renderer *renderer, GameObject *object) {

  useShaderProgram(object->shaderProgram);
  setBuffersRenderer(renderer, object);


  if (object->textures)
    bindTexturesUniform(object->shaderProgram, object->textures,
                        object->numTextures);

  mat4 mvp;
  getModelViewProj(renderer->camera, object, mvp);

  glUniformMatrix4fv(glGetUniformLocation(object->shaderProgram->id, "mvp"), 1,
                     GL_FALSE, mvp[0]);

  glBindVertexArray(renderer->vao);
  glBindBuffer(GL_ARRAY_BUFFER, renderer->vbo);

  glDrawArrays(GL_TRIANGLES, 0,object->mesh->numVertices );

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  glDeleteBuffers(1, &(renderer->vbo));
  glDeleteBuffers(1, &(renderer->ebo));

  unbindShaderProgram();
}

void freeRenderer(Renderer *renderer) {
  glDeleteVertexArrays(1, &(renderer->vao));

  glDeleteBuffers(1, &(renderer->vbo));
  glDeleteBuffers(1, &(renderer->ebo));
}