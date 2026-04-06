#include "learnopengl/engine/textures.h"
#include <glad/glad.h>
#include <learnopengl/engine/framebuffer.h>
#include <learnopengl/engine/shader.h>
#include <learnopengl/utils/stb_image_write.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>

const static float uv[12] = {0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0};

const static float quad[18] = {-1, -1, 0, -1, 1,  0, 1,  1,  0,
                               1,  1,  0, 1,  -1, 0, -1, -1, 0};

void initRenderBuffer(RenderBuffer *renderBuffer, uint attachment) {
  glGenRenderbuffers(1, &(renderBuffer->glID));
  renderBuffer->attachment = attachment;
}
void freeRenderBuffer(RenderBuffer *renderBuffer) {
  glDeleteRenderbuffers(1, &(renderBuffer->glID));
}

void initFrameBuffer(FrameBuffer *framebuffer,
                     uint numTextures, Texture** textures) {

  glGenFramebuffers(1, &(framebuffer->glID));
  framebuffer->width = 0;
  framebuffer->height = 0;
  framebuffer->numTextures = numTextures;
  framebuffer->textures = textures;
  initRenderBuffer(&(framebuffer->renderBuffer), GL_DEPTH24_STENCIL8);

  glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->glID);
  glBindRenderbuffer(GL_RENDERBUFFER, framebuffer->renderBuffer.glID);

  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                            GL_RENDERBUFFER, framebuffer->renderBuffer.glID);


  uint attachments[framebuffer->numTextures];
  for (int i =0; i <framebuffer->numTextures; i++) {
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i,
                           GL_TEXTURE_2D, framebuffer->textures[i]->glID, 0);
    attachments[i] = GL_COLOR_ATTACHMENT0 + i;
  }
  glDrawBuffers(framebuffer->numTextures, attachments);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glBindRenderbuffer(GL_RENDERBUFFER, 0);

  glGenVertexArrays(1, &framebuffer->vao);
  glGenBuffers(1, &framebuffer->quadVBO);
  glGenBuffers(1, &framebuffer->uvVBO);

  glBindVertexArray(framebuffer->vao);

  glBindBuffer(GL_ARRAY_BUFFER, framebuffer->quadVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);
  glVertexAttribPointer(ATTR_LOCATION_POS, 3, GL_FLOAT, GL_FALSE,
                        3 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(ATTR_LOCATION_POS);

  glBindBuffer(GL_ARRAY_BUFFER, framebuffer->uvVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(uv), uv, GL_STATIC_DRAW);
  glVertexAttribPointer(ATTR_LOCATION_UV, 2, GL_FLOAT, GL_FALSE,
                        2 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(ATTR_LOCATION_UV);

  glBindVertexArray(0);
}

void bindFrameBuffer(FrameBuffer *framebuffer, uint width, uint height) {

  glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->glID);

  if (width == framebuffer->width && height == framebuffer->height)
    return;

  // only resize buffers if change is found

  framebuffer->width = width;
  framebuffer->height = height;

  glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->glID);

  glBindTexture(GL_TEXTURE_2D, 0);

  glBindRenderbuffer(GL_RENDERBUFFER, framebuffer->renderBuffer.glID);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
  glBindRenderbuffer(GL_RENDERBUFFER, 0);

  for (int i = 0; i < framebuffer->numTextures; i++) {
    glBindTexture(GL_TEXTURE_2D, framebuffer->textures[i]->glID);
    framebuffer->textures[i]->width = width;
    framebuffer->textures[i]->height = height;
    uint format = framebuffer->textures[i]->format ;
    uint internalFormat = framebuffer->textures[i]->internalFormat;
    uint datatype = framebuffer->textures[i]->datatype;

    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, format,
                 datatype, NULL);
  }
  glBindTexture(GL_TEXTURE_2D, 0);

  uint status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  assert(status == GL_FRAMEBUFFER_COMPLETE);
}
void unbindFrameBuffer() {
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

const static char *uniformPrefix = "image";

static bool done = false;

void renderFrameBuffer(FrameBuffer *framebuffer, uint width, uint height,
                        ShaderProgram *shaderProgram,  const char ** names) {

  glDisable(GL_DEPTH_TEST);
  glDisable(GL_CULL_FACE);
  useShaderProgram(shaderProgram);
    // Texture t ;
    // initTexture(&t, "test", GL_TEXTURE_2D, GL_RGB, GL_RGB8);
    // loadTexture2D(&t, "./textures/container.jpg");

  // char uniformName[strlen(uniformPrefix) + 3];
  for (int i = 0; i < framebuffer->numTextures; i++) {
    // int t_id ;
    // glGetNamedFramebufferAttachmentParameteriv(framebuffer->glID,GL_COLOR_ATTACHMENT0+i, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME,&t_id );
    // assert(t_id == framebuffer->textures[i]->glID);
    // snprintf(uniformName, sizeof(uniformName), "image%d", i);
    glBindTextureUnit(i+1, framebuffer->textures[i]->glID);
    setUniform1i(shaderProgram, names[i], i+1);
  }

  glBindVertexArray(framebuffer->vao);
  glDrawArrays(GL_TRIANGLES, 0, 6);
  glBindVertexArray(0);

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  unbindShaderProgram();
}

void freeFrameBuffer(FrameBuffer *frameBuffer) {
  glDeleteVertexArrays(1, &frameBuffer->vao);
  glDeleteBuffers(1, &frameBuffer->quadVBO);
  glDeleteBuffers(1, &frameBuffer->uvVBO);
  for (int i = 0; i < frameBuffer->numTextures; i++) {
    freeTexture(frameBuffer->textures[i]);
  }
  if (frameBuffer->textures)
    free(frameBuffer->textures);
  freeRenderBuffer(&frameBuffer->renderBuffer);
  glDeleteFramebuffers(1, &frameBuffer->glID);
}
