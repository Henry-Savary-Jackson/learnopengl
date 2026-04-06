#include "learnopengl/engine/textures.h"
#include <glad/glad.h>
#include <learnopengl/engine/mesh.h>
#include <learnopengl/engine/shader.h>
#include <learnopengl/utils/stb_image.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <learnopengl/utils/utils.h>

void initTexture(Texture *texture, const char *name, uint textureType,uint format ,uint internalFormat, uint type) {
  texture->name = name;
  texture->textureType = textureType; // UNKNOWN VALUE
  texture->internalFormat = internalFormat;
  texture->format = format;
  texture->datatype = type;
  texture->depth = 0;
  glCreateTextures(textureType, 1, &(texture->glID));
}

void loadTexture2D(Texture *texture, const char *path) {
texture->textureType = GL_TEXTURE_2D;
texture->depth =1;

  stbi_set_flip_vertically_on_load(true);
  int nrChannels;
  unsigned char *data =
      stbi_load(path, (int*)&(texture->width), (int*)&(texture->height), &nrChannels, 0);
  if (data) {
    texture->format = nrChannels == 4 ? GL_RGBA : GL_RGB;
    texture->internalFormat = nrChannels == 4 ? GL_RGBA8 : GL_RGB8;
    texture->datatype = GL_UNSIGNED_BYTE;
    glTextureStorage2D(texture->glID, 1, texture->internalFormat,
                       texture->width, texture->height);
    glTextureSubImage2D(texture->glID, 0, 0, 0, texture->width, texture->height,
                        texture->format, texture->datatype,
                        data);

    stbi_image_free(data);

  } else {
    printf("Failed to load texture!!!!");
  }
}

void bindTexture(Texture* texture){
  glBindTexture(texture->textureType, texture->glID);
}

void unbindTexture(Texture* texture){
  glBindTexture(texture->textureType, 0);
}

void loadTextureCubeMap(Texture *texture, const char *path,
                        const char *sides[6]) {

  texture->textureType = GL_TEXTURE_CUBE_MAP;
  texture->depth = 6;

  stbi_set_flip_vertically_on_load(false);

  for (int i = 0; i < 6; i++) {
    int nrChannels;
    char* fullpath [PATH_MAX]; // TODO CHANGE LEGNTH TO MATCH SEPARATOR
    strcpy((char*)fullpath, path);
    char pathSeparator[2] ;
    pathSeparator[0] = separator();
    pathSeparator[1] = 0;
    strcat((char*)fullpath, pathSeparator);// TODO: make this a general line separator for the os
    strcat((char*)fullpath, sides[i]);


    unsigned char *data =
        stbi_load((char*)fullpath, (int*)&texture->width, (int*)&texture->height, &nrChannels, 0);

    texture->format = nrChannels == 4 ? GL_RGBA : GL_RGB;
    texture->internalFormat = nrChannels == 4 ? GL_RGBA8 : GL_RGB8;
    texture->datatype = GL_UNSIGNED_BYTE;
    if (i == 0)
      glTextureStorage2D(texture->glID, 1, texture->internalFormat,
                        texture->width, texture->height);
    glTextureSubImage3D(texture->glID, 0,0,0,i,texture->width, texture->height, 1  ,texture->format , texture->datatype, data);

  }
}



void freeTexture(Texture *texture) { glDeleteTextures(1, &(texture->glID)); }

void initSampler(Sampler *sampler) { glGenSamplers(1, &(sampler->glID)); }

void setTextureParameter(Texture *texture, uint key, uint value){
  glTextureParameteri(texture->glID, key, value);
}

void setSamplerParameter(Sampler *sampler, uint key, uint value) {
  glSamplerParameteri(sampler->glID, key, value);
}
void freeSampler(Sampler *sampler) { glDeleteSamplers(1, &(sampler->glID)); }
