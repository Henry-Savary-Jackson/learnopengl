#include "utils.h"
#include <glad/glad.h>
#include <stddef.h>
#include <stdio.h>

int createShaderProgram() {

  unsigned int shaderProgram = glCreateProgram();
  // is there any additional config???

  return shaderProgram;
}

int createShaderFromFile(const char *path, GLenum type) {
  FILE *fd = fopen(path, "r");

  if (fd == NULL) {
    printf("Could not read file %s", path);
    return 0;
  }

  fseek(fd, 0L, SEEK_END);
  long length = ftell(fd);

  fseek(fd, 0L, SEEK_SET);

  char data[length + 1];

  fread(data, sizeof(GLchar), length, fd);
  data[length] = 0;

  fclose(fd);

  GLuint shader = glCreateShader(type);
  GLint result;

  const char *data_ptr = (const char *)data;

  glShaderSource(shader, 1, (const char *const *)(&data_ptr), NULL);

  glCompileShader(shader);
  glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
  if (!result) {
    printf("Could not compile shader!");
    return 0;
  }

  return shader;
}

void attachShadersToProgram(GLuint program, GLuint shaders[], int length) {
  for (int i = 0; i < length; i++) {
    glAttachShader(program, shaders[i]);
  }
  glLinkProgram(program);
  GLint result;
  glGetProgramiv(program, GL_LINK_STATUS, &result);
  if (!result) {
    printf("Could not link shader!");
  }
}