
#pragma  once
#include <glad/glad.h>

int createShaderProgram() ;

int createShaderFromFile(const char *path, GLenum type ) ;

void attachShadersToProgram(GLuint program, GLuint shaders[], int length) ;