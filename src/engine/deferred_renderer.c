#include "learnopengl/engine/batch_render.h"
#include "learnopengl/engine/camera.h"
#include "learnopengl/engine/framebuffer.h"
#include "learnopengl/engine/global_info.h"
#include "learnopengl/engine/light.h"
#include "learnopengl/engine/mesh.h"
#include "learnopengl/engine/object.h"
#include "learnopengl/engine/render_context.h"
#include "learnopengl/engine/scene.h"
#include "learnopengl/engine/shader.h"
#include "learnopengl/engine/textures.h"
#include <cglm/mat4.h>
#include <glad/glad.h>
#include <learnopengl/engine/deferred_light_renderer.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>


void setUpBuffersDeferred(DeferredLightRenderer *renderer);

void initDeferredLightRenderer(DeferredLightRenderer *renderer,
                               uint maxLights) {
  renderer->maxLights = maxLights;
  renderer->meshes = malloc(sizeof(Mesh) * maxLights);
  renderer->meshIndices = malloc(sizeof(uint) * maxLights);
  renderer->lights = malloc(sizeof(LightSource *) * maxLights);
  renderer->lightDataBuffer = malloc(sizeof(LightData) * maxLights);
  renderer->vertexBuffer =
      malloc((sizeof(Vertex) * 1000) * maxLights);
  renderer->transformBuffer = malloc(sizeof(mat4) * maxLights);
  renderer->eboBuffer = malloc((sizeof(uint) * 1000) * maxLights);

  renderer->numLights = 0;
  renderer->numVertices = 0;
  renderer->numElements = 0;
  renderer->numModels = 0;
  setUpBuffersDeferred(renderer);
}

void setUpBuffersDeferred(DeferredLightRenderer *renderer) {
  size_t colSize = sizeof(vec4);
  size_t mat4Size = sizeof(mat4);
  glGenVertexArrays(1, &renderer->vao);
  glBindVertexArray(renderer->vao);
  glGenBuffers(1, &renderer->vertexVBO);
  glGenBuffers(1, &renderer->lightIndexVBO);
  glGenBuffers(1, &renderer->transformVBO);
  glGenBuffers(1, &renderer->vertexEBO);

  uint maxVertices = 500 * renderer->maxLights;
  size_t modelsOffset = maxVertices * sizeof(Vertex);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer->vertexEBO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  glBindBuffer(GL_ARRAY_BUFFER, renderer->transformVBO);

  glVertexAttribPointer(ATTR_LOCATION_MODEL, 4, GL_FLOAT, GL_FALSE, mat4Size,
                        (void *)0);
  glVertexAttribPointer(ATTR_LOCATION_MODEL + 1, 4, GL_FLOAT, GL_FALSE,
                        mat4Size, (void *)colSize );
  glVertexAttribPointer(ATTR_LOCATION_MODEL + 2, 4, GL_FLOAT, GL_FALSE,
                        mat4Size, (void *)(2 * colSize ));
  glVertexAttribPointer(ATTR_LOCATION_MODEL + 3, 4, GL_FLOAT, GL_FALSE,
                        mat4Size, (void *)(3 * colSize ));

  glEnableVertexAttribArray(ATTR_LOCATION_MODEL);
  glVertexAttribDivisor(ATTR_LOCATION_MODEL, 1);
  glEnableVertexAttribArray(ATTR_LOCATION_MODEL + 1);
  glVertexAttribDivisor(ATTR_LOCATION_MODEL + 1, 1);
  glEnableVertexAttribArray(ATTR_LOCATION_MODEL + 2);
  glVertexAttribDivisor(ATTR_LOCATION_MODEL + 2, 1);
  glEnableVertexAttribArray(ATTR_LOCATION_MODEL + 3);
  glVertexAttribDivisor(ATTR_LOCATION_MODEL + 3, 1);


  glBindBuffer(GL_ARRAY_BUFFER, renderer->vertexVBO);
  // create shader attributes
  glVertexAttribPointer(ATTR_LOCATION_POS, 3, GL_FLOAT, GL_FALSE,
                        sizeof(Vertex), (void *)offsetof(Vertex, pos));

  glEnableVertexAttribArray(ATTR_LOCATION_POS); // vertex position attr

  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void setLights(DeferredLightRenderer *renderer, LightSource **lightSource,
               uint numLights, Mesh *mesh) {
  renderer->meshes[0] = *mesh;
  renderer->numElements = mesh->numElements;
  renderer->numVertices = mesh->numVertices;
  memcpy(renderer->eboBuffer, mesh->elementOrder,
         sizeof(uint) * mesh->numElements);
  memcpy(renderer->vertexBuffer, mesh->vertices,
         sizeof(Vertex) * mesh->numVertices);
  memcpy(renderer->lights, lightSource, numLights * sizeof(LightSource **));
  for (int i = 0; i < numLights; i++) {
    getModelMatrix(&renderer->lights[i]->object,renderer->transformBuffer[i]);
  }
  renderer->numLights = numLights;
}

void sendLightData(DeferredLightRenderer *renderer) {
  glNamedBufferData(renderer->vertexVBO,
                    renderer->numVertices * sizeof(Vertex) ,
                    renderer->vertexBuffer, GL_STATIC_DRAW);
  glNamedBufferData(renderer->transformVBO,
                    renderer->numLights * sizeof(mat4) ,
                    renderer->transformBuffer, GL_STATIC_DRAW);
  glNamedBufferData(renderer->vertexEBO, renderer->numElements * sizeof(uint),
                    renderer->eboBuffer, GL_STATIC_DRAW);
}

void renderAllLightVolumes(DeferredLightRenderer *renderer) {
  // send all data to GPU

  glBindVertexArray(renderer->vao);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer->vertexEBO);

  glDrawElementsInstanced(GL_TRIANGLES, renderer->numElements, GL_UNSIGNED_INT,
                          NULL, renderer->numLights);

  // draw call using instancing

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
  unbindShaderProgram();
}

void initDeferredRenderer(DeferredRenderer *renderer,
                          const char **gBufferTextureNames,
                          uint numGBufferTextures, ShaderProgram *geom,
                          ShaderProgram *light, uint maxLights) {
  renderer->gBufferTextureNames = gBufferTextureNames;
  renderer->numGBufferTextures = numGBufferTextures;

  renderer->gBufferTextures = malloc(sizeof(Texture*) * numGBufferTextures);

  for (int i = 0; i < numGBufferTextures; i++) {
    renderer->gBufferTextures[i] = malloc(sizeof(Texture));
    initTexture(renderer->gBufferTextures[i], gBufferTextureNames[i],
                GL_TEXTURE_2D, GL_RGB, GL_RGB32F, GL_FLOAT);
    setTextureParameter(renderer->gBufferTextures[i], GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    setTextureParameter(renderer->gBufferTextures[i], GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  }
  // // index of material index, store only in there
  initTexture(renderer->gBufferTextures[5], gBufferTextureNames[5],
              GL_TEXTURE_2D, GL_RED, GL_R8, GL_UNSIGNED_BYTE);

  initFrameBuffer(&renderer->gBuffer, numGBufferTextures,
                  renderer->gBufferTextures);

  initDeferredLightRenderer(&renderer->lightVolumeRenderer, maxLights);

  renderer->shaderProgramGeom = geom;
  renderer->shaderProgramLighting = light;

  initSampler(&renderer->sampler);
  setSamplerParameter(&renderer->sampler, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  setSamplerParameter(&renderer->sampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
}

void renderDeferred(DeferredRenderer *renderer, Scene *scene, ShaderProgram* shaderProgram) {

  // draw scene to gbuffer
  bindFrameBuffer(&renderer->gBuffer, scene->camera->width,
                  scene->camera->height);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  renderLoop(scene);
  uint width = scene->camera->width; uint height= scene->camera->height;


  glBindFramebuffer(GL_READ_FRAMEBUFFER, renderer->gBuffer.glID);
  glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
  glBlitFramebuffer(0,0,width, height, 0,0, width, height, GL_DEPTH_BUFFER_BIT,GL_NEAREST );

  // renderFrameBuffer(&renderer->gBuffer , scene->camera->width, scene->camera->height,shaderProgram , renderer->gBufferTextureNames);
//   // draw everything to screen now

//   // first pass determines values in stencil buffer

  setLights(&renderer->lightVolumeRenderer, scene->lights, scene->numLights,
            scene->lights[0]->object.mesh);

  sendLightData(&renderer->lightVolumeRenderer);

  glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
  glDepthMask(GL_FALSE);
  glClear(GL_STENCIL_BUFFER_BIT); // reset stencil to zero everywhere

  useShaderProgram(renderer->shaderProgramGeom);

  attachContextToShader(&scene->renderingContext, renderer->shaderProgramGeom);

// //   // We need the stencil test to be enabled but we want it
//   // to succeed always. Only the depth test matters.
  glEnable(GL_STENCIL_TEST);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glStencilFuncSeparate(GL_FRONT,GL_ALWAYS, 0, 0);
  glStencilOpSeparate(GL_FRONT, GL_KEEP, GL_INCR, GL_KEEP);


  renderAllLightVolumes(&renderer->lightVolumeRenderer);

// // //   // second pass, now only perform lighting calculations on fragments whose
// // //   // stencil value is not zero the lighting caclulations are now done batched
// // //   // and in parallel on the GPU and only on pixels that matter
  useShaderProgram(renderer->shaderProgramLighting);
  attachContextToShader(&scene->renderingContext, renderer->shaderProgramLighting);

// //   // additive blending on each light's contribution
  glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

  glDisable(GL_BLEND);
  // glBlendFunc(GL_ONE, GL_ONE);
  // glBlendEquation(GL_FUNC_ADD);
  glDepthFunc(GL_GREATER);

  glCullFace(GL_FRONT);

  glStencilFunc(GL_EQUAL, 0, 0xff);
  glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

  for (int i = 0; i < renderer->numGBufferTextures; i++) {
    glBindTextureUnit(i+1, renderer->gBufferTextures[i]->glID);
    setUniform1i(renderer->shaderProgramLighting,
                 renderer->gBufferTextureNames[i], i+1);
  }

  useShaderProgram(renderer->shaderProgramLighting);

  // final pass
  renderAllLightVolumes(&renderer->lightVolumeRenderer);

  // glClear(GL_COLOR_BUFFER_BIT |GL_DEPTH_BUFFER_BIT); // reset stencil to zero everywhere

  // re-enable defaults
  glCullFace(GL_BACK);
  glDepthMask(GL_TRUE);
  glDepthFunc(GL_LESS);
}
