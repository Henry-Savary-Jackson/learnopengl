#include "config.h"
#include "mesh.h"
#include "shader_utils.h"
#include "utils.h"
#include <glad/glad.h>
#include <GL/gl.h>
#include <GL/glext.h>
#include <stdbool.h>
#include <stdio.h>
#include <GLFW/glfw3.h>

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
}

float vertices[] = {0.5f, 0.5f,  0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
                    0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
                    0.1f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
                    0.1f, 0.5f,  0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f};

int elements[] = {
    // note that we start from 0!
    0, 1,
    3, // first triangle
    1, 2,
    3 // second triangle

};

int main() {

  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
  // glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

  GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, TITLE, NULL, NULL);
  if (window == NULL) {
    fprintf(stdout, "Failed to make window!");
    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(window);
  // GLint glewResult = glewInit();
  // if (glewResult != GLEW_OK) {
  //   fprintf(stdout, "Error: %s\n", glewGetErrorString(glewResult));
  //   exit(1);
  // }

  glViewport(0, 0, WIDTH, HEIGHT);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  ShaderProg shaderProgram;
  initShaderProgram(&shaderProgram);
  Shader vertexShader = {.type = GL_VERTEX_SHADER};
  Shader fragmentShader = {.type = GL_FRAGMENT_SHADER};
  initShaderFromFile(&vertexShader, "./shaders/vertex.glsl");
  initShaderFromFile(&vertexShader, "./shaders/fragment.glsl");
  Shader shaders[] = {vertexShader, fragmentShader};
  attachShadersToProgram(&shaderProgram, shaders, 2);

  Mesh mesh;
  initMesh(&mesh, (Vertex *)vertices, (int)(sizeof(vertices) / sizeof(Vertex)),
           elements, LENGTH(elements));
  setBuffersMesh(&mesh);

  useShaderProgram(&shaderProgram);

  while (!glfwWindowShouldClose(window)) {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    drawMesh(&mesh);

    glfwSwapBuffers(window);
    glfwPollEvents();

  }

  freeMesh(&mesh);

  freeShaderProgram(&shaderProgram);

  glfwTerminate();
  return 0;
}