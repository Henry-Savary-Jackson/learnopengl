#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include "stdlib.h"
#include <learnopengl/engine/shader.h>
#include <glad/glad.h>


void initShaderProgram(ShaderProgram* shaderProg) {
  shaderProg->id = glCreateProgram();
  shaderProg->shaders = NULL;
}


void initShaderProgramFromFiles(ShaderProgram* shaderProgram, const char * vsPath, const char * fsPath){
  initShaderProgram(shaderProgram);
  Shader vsShader = {.type = GL_VERTEX_SHADER};
  initShaderFromFile(&vsShader, vsPath);
  Shader fsShader = {.type = GL_FRAGMENT_SHADER};
  initShaderFromFile(&fsShader, fsPath);
  attachShadersToProgram(shaderProgram, (Shader[2]){vsShader, fsShader}, 2);
}

void initShaderFromFile(Shader* shader,const char *path) {
  FILE *fd = fopen(path, "r");

  const char * cwd = getenv("PWD");


  if (fd == NULL) {
    printf("Could not read file %s", path);
  }
  assert(fd);

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
  }
  assert(result);
}

void setShaderAttributeLocations(ShaderProgram* program){
  for (int i =0; i < sizeof(attributes)/sizeof(attributeSpec); i++) {
    glBindAttribLocation(program->id, attributes[i].location, attributes[i].name);
    // uint loc = glGetAttribLocation(program->id, attributes[i].name);
    // if ( loc != GL_INVALID_INDEX)
  }
}

void attachShadersToProgram(ShaderProgram* program, Shader shaders[], int length) {
  program->shaders = shaders;
  program->num_shaders=length;
  for (int i = 0; i < length; i++) {
    glAttachShader(program->id, shaders[i].id);
  }

  // make sure all the attributes we want can be accesed by the shader

  setShaderAttributeLocations(program);
  glLinkProgram(program->id);
  GLint result;
  glGetProgramiv(program->id, GL_LINK_STATUS, &result);
  if (!result) {
    printf("Could not link shader!");
  }
  // assert(result);
}


void useShaderProgram(ShaderProgram* shaderProg){
  glUseProgram(shaderProg->id);
}

void unbindShaderProgram(){
  glUseProgram(0);
}


void freeShader(Shader* shader){
  glDeleteShader(shader->id);
}

void freeShaderProgram(ShaderProgram* shaderProg){
  glDeleteProgram(shaderProg->id);
  for (int i =0; i<shaderProg->num_shaders; i++) {
    // freeShader(shaderProg->shaders + i);
  }
}
