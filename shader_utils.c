#include <stddef.h>
#include <stdio.h>
#include "stdlib.h"
#include "shader_utils.h"
#include <glad/glad.h>
#include <GL/gl.h>


void initShaderProgram(ShaderProg* shaderProg) {
  shaderProg->id = glCreateProgram();
  shaderProg->shaders = NULL; 
}

void initShaderFromFile(Shader* shader,const char *path) {
  FILE *fd = fopen(path, "r");

  if (fd == NULL) {
    printf("Could not read file %s", path);
    return ;
  }

  fseek(fd, 0L, SEEK_END);
  long length = ftell(fd);

  fseek(fd, 0L, SEEK_SET);

  char data[length + 1];

  fread(data, sizeof(GLchar), length, fd);
  data[length] = 0;

  fclose(fd);


  shader->id = glCreateShader(shader->type);
  GLint result;

  const char *data_ptr = (const char *)data;

  glShaderSource(shader->id, 1, (const char *const *)(&data_ptr), NULL);

  glCompileShader(shader->id);
  glGetShaderiv(shader->id, GL_COMPILE_STATUS, &result);
  if (!result) {
    printf("Could not compile shader!");
    return ;
  }
}

void attachShadersToProgram(ShaderProg* program, Shader shaders[], int length) {
  program->shaders = shaders;
  program->num_shaders=length;
  for (int i = 0; i < length; i++) {
    glAttachShader(program->id, shaders[i].id);
  }
  glLinkProgram(program->id);
  GLint result;
  glGetProgramiv(program->id, GL_LINK_STATUS, &result);
  if (!result) {
    printf("Could not link shader!");
  }
}


void useShaderProgram(ShaderProg* shaderProg){
  glUseProgram(shaderProg->id);
}


void freeShader(Shader* shader){
  glDeleteShader(shader->id);
}

void freeShaderProgram(ShaderProg* shaderProg){
  glDeleteProgram(shaderProg->id);
  for (int i =0; i<shaderProg->num_shaders; i++) {
    freeShader(shaderProg->shaders + i);
  }
}