#include "learnopengl/engine/textures.h"
#include <learnopengl/engine/shader.h>
#include <learnopengl/utils/stb_image.h>
#include <glad/glad.h>
#include <GL/glext.h>
#include <GL/gl.h>
#include <learnopengl/engine/mesh.h>
#include <stdio.h>

void initTexture(Texture *texture, const char *name, const char *path) {

  texture->name = name;
  glCreateTextures(GL_TEXTURE_2D,1, &(texture->glID));

  stbi_set_flip_vertically_on_load(true);
  int nrChannels;
  unsigned char *data =
      stbi_load(path, &(texture->width), &(texture->height), &nrChannels, 0);
  if (data) {
    glTextureStorage2D(texture->glID, 1, nrChannels == 4?GL_RGBA8: GL_RGB8, texture->width, texture->height  );
    glTextureSubImage2D(texture->glID, 0, 0, 0 ,texture->width, texture->height, nrChannels == 4?GL_RGBA: GL_RGB,  GL_UNSIGNED_BYTE, data);

    stbi_image_free(data);

  } else {
    printf("Failed to load texture!!!!");
  }
}

void freeTexture(Texture *texture) { glDeleteTextures(1, &(texture->glID)); }


void initSampler(Sampler *sampler){
  glGenSamplers(1,&(sampler->glID));
}

void setSamplerParameter(Sampler* sampler, uint key, uint value){
  glSamplerParameteri(sampler->glID, key, value);
}
void freeSampler(Sampler* sampler){
  glDeleteSamplers(1, &(sampler->glID));
}
