#include "config.h"
#include "utils.h"

#include "shader_utils.h"

#include <GL/glew.h>
#include <GL/gl.h>

#include <GL/glext.h>
#include <stdbool.h>
#include <stdio.h>

#include <GLFW/glfw3.h>


void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
}

float vertices[] = {
    0.5f, 0.5f, 0.0f, 0.5f, -0.5f, 0.0f, 0.1f, -0.5f, 0.0f, 0.1f, 0.5f, 0.0f,
};

float colors[] = {
    0.0f, 1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
};

unsigned int elements[] = {
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
  glfwMakeContextCurrent(window);
  if (window == NULL) {
    printf("Failed to make window!");
    glfwTerminate();
    return -1;
  }

  glViewport(0, 0, WIDTH, HEIGHT);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  ShaderProg shaderProgram;
  initShaderProgram(&shaderProgram);
  Shader vertexShader=  {.type=GL_VERTEX_SHADER};
  Shader fragmentShader=  {.type=GL_FRAGMENT_SHADER};
  initShaderFromFile(&vertexShader,"./shaders/vertex.glsl");
  initShaderFromFile(&vertexShader,"./shaders/fragment.glsl");
  Shader shaders[] = {vertexShader, fragmentShader};
  attachShadersToProgram(&shaderProgram, shaders, 2);

  unsigned int EBO;
  unsigned int VAO;
  unsigned int colVBO;
  unsigned int VBO;

  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &colVBO);
  glGenBuffers(1, &EBO);

  glBindVertexArray(VAO);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elements), elements,
               GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, VBO);

  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);

  glEnableVertexAttribArray(0);

  glBindBuffer(GL_ARRAY_BUFFER, colVBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(1);

  useShaderProgram(&shaderProgram);

  while (!glfwWindowShouldClose(window)) {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glDrawElements(GL_TRIANGLES, LENGTH(elements), GL_UNSIGNED_INT, 0);

    glfwSwapBuffers(window);

    glfwPollEvents();
  }


  glDeleteVertexArrays(1, &VAO);
  glDeleteBuffers(1, &VBO);
  glDeleteBuffers(1, &EBO);

  freeShaderProgram(&shaderProgram);

  glfwTerminate();
  return 0;
}