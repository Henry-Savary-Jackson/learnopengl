#include "stdlib.h"
#include <assert.h>
#include <glad/glad.h>
#include <learnopengl/engine/shader.h>
#include <stddef.h>
#include <stdio.h>

void initShaderProgram(ShaderProgram *shaderProg) {
  shaderProg->id = glCreateProgram();
  shaderProg->shaders = NULL;
}

uint getUniformLocation(ShaderProgram *shaderProgram, const char *name) {
  return glGetUniformLocation(shaderProgram->id, name);
}

uint setUniform1f(ShaderProgram *shaderProgram, const char *name, float value) {
  uint location = getUniformLocation(shaderProgram, name);
  if (location != GL_INVALID_INDEX) {
    glUniform1f(location, value);
    return 0;
  }
  return -1;
}
uint setUniform1i(ShaderProgram *shaderProgram, const char *name, int value) {
  uint location = getUniformLocation(shaderProgram, name);
  if (location != GL_INVALID_INDEX) {

    glUniform1i(location, value);
    return 0;
  }
  return -1;
}

uint setUniform1ui(ShaderProgram *shaderProgram, const char *name, uint value) {
  uint location = getUniformLocation(shaderProgram, name);
  if (location != GL_INVALID_INDEX) {

    glUniform1ui(location, value);
    return 0;
  }
  return -1;
}

uint setUniform3fv(ShaderProgram *shaderProgram, const char *name,
                   float value[3]) {
  uint location = getUniformLocation(shaderProgram, name);
  if (location != GL_INVALID_INDEX) {
    glUniform3f(location, value[0], value[1], value[2]);
    return 0;
  }
  return -1;
}
uint setUniform4fv(ShaderProgram *shaderProgram, const char *name,
                   float value[4]) {
  uint location = getUniformLocation(shaderProgram, name);
  if (location != GL_INVALID_INDEX) {
    glUniform4f(location, value[0], value[1], value[2], value[3]);
    return 0;
  }
  return -1;
}

void initShaderProgramFromFiles(ShaderProgram *shaderProgram,
                                const char *vsPath, const char *fsPath) {
  initShaderProgram(shaderProgram);
  Shader vsShader = {.type = GL_VERTEX_SHADER};
  initShaderFromFile(&vsShader, vsPath);
  Shader fsShader = {.type = GL_FRAGMENT_SHADER};
  initShaderFromFile(&fsShader, fsPath);
  attachShadersToProgram(shaderProgram, (Shader[2]){vsShader, fsShader}, 2);
}

void initShaderFromFile(Shader *shader, const char *path) {
  FILE *fd = fopen(path, "r");

  const char *cwd = getenv("PWD");

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
  shader->path = path;
  assert(result);
}

void setShaderAttributeLocations(ShaderProgram *program) {
  for (int i = 0; i < sizeof(attributes) / sizeof(attributeSpec); i++) {
    glBindAttribLocation(program->id, attributes[i].location,
                         attributes[i].name);
  }
}

void attachShadersToProgram(ShaderProgram *program, Shader shaders[],
                            int length) {
  program->shaders = shaders;
  program->num_shaders = length;
  for (int i = 0; i < length; i++) {
    glAttachShader(program->id, shaders[i].id);
  }

  // make sure all the attributes we want can be accesed by the shader

  setShaderAttributeLocations(program);
  glLinkProgram(program->id);
  GLint result;
  glGetProgramiv(program->id, GL_LINK_STATUS, &result);
  if (!result) {
    int lengths = 1024;
    char log[lengths+1];
    glGetProgramInfoLog(program->id, lengths+1, &lengths,  log );
    printf("Could not link shaders with paths \'%s\' \'%s\'!\nInfo Log:%s", program->shaders[0].path, program->shaders[1].path , log);

  }
  assert(result);
  // assert(result);
}

void useShaderProgram(ShaderProgram *shaderProg) {
  glUseProgram(shaderProg->id);
}

void unbindShaderProgram() { glUseProgram(0); }

void freeShader(Shader *shader) { glDeleteShader(shader->id); }

void freeShaderProgram(ShaderProgram *shaderProg) {
  glDeleteProgram(shaderProg->id);
}
