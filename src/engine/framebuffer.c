#include "learnopengl/engine/textures.h"
#include <glad/glad.h>
#include <learnopengl/engine/framebuffer.h>
#include <learnopengl/engine/shader.h>
#include <stdio.h>

const static float uv[12] = {
  0, 0,
  0, 1,
  1, 1,
  1, 1,
  1, 0,
   0, 0};

const static float quad[18] = {
  -1, -1, 0,
  -1, 1,  0,
  1,  1,  0,
  1,  1,  0,
  1,  -1, 0,
  -1, -1, 0};

void initRenderBuffer(RenderBuffer *renderBuffer, uint attachment) {
  glGenRenderbuffers(1, &(renderBuffer->glID));
  renderBuffer->attachment = attachment;
}
void freeRenderBuffer(RenderBuffer *renderBuffer) {
  glDeleteRenderbuffers(1, &(renderBuffer->glID));
}

void initFrameBuffer(FrameBuffer *framebuffer, uint width, uint height) {

  glGenFramebuffers(1, &(framebuffer->glID));
  framebuffer->width = width;
  framebuffer->height = height;
  initRenderBuffer(&(framebuffer->renderBuffer), GL_DEPTH24_STENCIL8);


  glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->glID);
  glBindRenderbuffer(GL_RENDERBUFFER, framebuffer->renderBuffer.glID);

  glFramebufferRenderbuffer(
      GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
      GL_RENDERBUFFER , framebuffer->renderBuffer.glID);


  initTexture(&framebuffer->texture, "lol");
  glGenTextures(1, &framebuffer->texture.glID);
  glBindTexture(GL_TEXTURE_2D, framebuffer->texture.glID);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,  framebuffer->texture.glID , 0);
  glBindTexture(GL_TEXTURE_2D, 0);

  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  glGenVertexArrays(1, &framebuffer->vao);
  glGenBuffers(1, &framebuffer->quadVBO);
  glGenBuffers(1, &framebuffer->uvVBO);

  glBindVertexArray(framebuffer->vao);

  glBindBuffer(GL_ARRAY_BUFFER, framebuffer->quadVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);
  glVertexAttribPointer(ATTR_LOCATION_POS, 3, GL_FLOAT, GL_FALSE,
                        3 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(ATTR_LOCATION_POS);

  glBindBuffer(GL_ARRAY_BUFFER, framebuffer->uvVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(uv), uv, GL_STATIC_DRAW);
  glVertexAttribPointer(ATTR_LOCATION_UV, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float),
                        (void*)0);
  glEnableVertexAttribArray( ATTR_LOCATION_UV);

  glBindVertexArray(0);

}

void bindFrameBuffer(FrameBuffer* framebuffer, uint width, uint height){

  glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->glID);


  glBindRenderbuffer(GL_RENDERBUFFER, framebuffer->renderBuffer.glID);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
  glBindRenderbuffer(GL_RENDERBUFFER, 0);

  glBindTexture(GL_TEXTURE_2D, framebuffer->texture.glID);

  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height,0,GL_RGBA, GL_UNSIGNED_BYTE ,NULL );
  glBindTexture(GL_TEXTURE_2D, 0);
  uint status = glCheckFramebufferStatus(GL_FRAMEBUFFER) ;
  assert(status == GL_FRAMEBUFFER_COMPLETE);

}
void unbindFrameBuffer(FrameBuffer* frameBuffer){
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void renderFrameBuffer(FrameBuffer *framebuffer, uint width, uint height) {

  glDisable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);
  useShaderProgram(framebuffer->shaderProgram);

  glBindTexture(GL_TEXTURE_2D, framebuffer->texture.glID);
  glUniform1i(glGetUniformLocation(framebuffer->shaderProgram->id, "tex"), 0);

  glBindVertexArray(framebuffer->vao);
  glDrawArrays(GL_TRIANGLE_FAN, 0, 6);
  glBindVertexArray(0);

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  unbindShaderProgram();
}
void attachShaderProgramToFrameBuffer(FrameBuffer *framebuffer,
                                      ShaderProgram *shaderProgram) {
  framebuffer->shaderProgram = shaderProgram;
}
void freeFrameBuffer(FrameBuffer *frameBuffer) {
  glDeleteVertexArrays(1, &frameBuffer->vao);
  glDeleteBuffers(1, &frameBuffer->quadVBO);
  glDeleteBuffers(1, &frameBuffer->uvVBO);
  freeTexture(&frameBuffer->texture);
  freeRenderBuffer(&frameBuffer->renderBuffer);
  glDeleteFramebuffers(1, &frameBuffer->glID);
}
