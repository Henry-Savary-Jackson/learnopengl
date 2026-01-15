#include "shader.h"
#include "stb_image.h"
#include <glad/glad.h>
#include <GL/glext.h>
#include <GL/gl.h>
#include "mesh.h"
#include <stdio.h>
#include <string.h>

void initTexture(Texture *texture, const char *name, const char *path) {

  texture->name = name;
  glGenTextures(1, &(texture->id));
  glBindTexture(GL_TEXTURE_2D, texture->id);


  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                  GL_NEAREST_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  stbi_set_flip_vertically_on_load(true);
  int nrChannels;
  unsigned char *data =
      stbi_load(path, &(texture->width), &(texture->height), &nrChannels, 0);
  if (data) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture->width, texture->height, 0,
                 nrChannels == 4?GL_RGBA: GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);

  } else {
    printf("Failed to load texture!!!!");
  }
  glBindTexture(GL_TEXTURE_2D, 0);
}

void bindTexture(Texture* texture){
    glBindTexture(GL_TEXTURE_2D, texture->id);
}

void freeTexture(Texture *texture) { glDeleteTextures(1, &(texture->id)); }

void bindTexturesUniform(ShaderProgram *shaderProgram, Texture *textures,
                         int numTextures) {
  // set unifroms for all the textures
  useShaderProgram(shaderProgram);
  for (int i = 0; i < numTextures; i++) {
    glActiveTexture(GL_TEXTURE0 + i);
    glBindTexture(GL_TEXTURE_2D, textures[i].id);
    // set sampler2d uniform
    char uniformName[strlen("texture") + 3];
    sprintf(uniformName, "texture%d", i);
    uint location = glGetUniformLocation(shaderProgram->id, uniformName);
    glUniform1i(location, i);
  }
  unbindShaderProgram();
}