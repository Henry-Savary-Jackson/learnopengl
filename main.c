#include "config.h"
#include "mesh.h"
#include "shader_utils.h"
#include "textures.h"
#include "utils.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cglm/affine-post.h>
#include <cglm/cam.h>
#include <cglm/mat4.h>
#include <stdbool.h>
#include <stdio.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
}

float vertices[] = {
    // positions          // colors           // texture coords
    0.5f,  0.5f,  0.0f, 1.0f, 0.0f, 0.0f, 2.0f, 2.0f, // top right
    0.5f,  -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 2.0f, 0.0f, // bottom right
    -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom left
    -0.5f, 0.5f,  0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 2.0f  // top left
};

int elements[] = {
    // note that we start from 0!
    0, 1,
    3, // first triangle
    1, 2,
    3 // second triangle

};

void APIENTRY glDebugOutput(GLenum source, GLenum type, unsigned int id,
                            GLenum severity, GLsizei length,
                            const char *message, const void *userParam) {
  // ignore non-significant error/warning codes
  if (id == 131169 || id == 131185 || id == 131218 || id == 131204)
    return;

  printf("---------------\n");
  printf("Debug message (%d) %s", id, message);

  switch (source) {
  case GL_DEBUG_SOURCE_API:
    printf("Source: API");
    break;
  case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
    printf("Source: Window System");
    break;
  case GL_DEBUG_SOURCE_SHADER_COMPILER:
    printf("Source: Shader Compiler");
    break;
  case GL_DEBUG_SOURCE_THIRD_PARTY:
    printf("Source: Third Party");
    break;
  case GL_DEBUG_SOURCE_APPLICATION:
    printf("Source: Application");
    break;
  case GL_DEBUG_SOURCE_OTHER:
    printf("Source: Other");
    break;
  }
  printf("\n");

  switch (type) {
  case GL_DEBUG_TYPE_ERROR:
    printf("Type: Error");
    break;
  case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
    printf("Type: Deprecated Behaviour");
    break;
  case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
    printf("Type: Undefined Behaviour");
    break;
  case GL_DEBUG_TYPE_PORTABILITY:
    printf("Type: Portability");
    break;
  case GL_DEBUG_TYPE_PERFORMANCE:
    printf("Type: Performance");
    break;
  case GL_DEBUG_TYPE_MARKER:
    printf("Type: Marker");
    break;
  case GL_DEBUG_TYPE_PUSH_GROUP:
    printf("Type: Push Group");
    break;
  case GL_DEBUG_TYPE_POP_GROUP:
    printf("Type: Pop Group");
    break;
  case GL_DEBUG_TYPE_OTHER:
    printf("Type: Other");
    break;
  };
  printf("\n");

  switch (severity) {
  case GL_DEBUG_SEVERITY_HIGH:
    printf("Severity: high");
    break;
  case GL_DEBUG_SEVERITY_MEDIUM:
    printf("Severity: medium");
    break;
  case GL_DEBUG_SEVERITY_LOW:
    printf("Severity: low");
    break;
  case GL_DEBUG_SEVERITY_NOTIFICATION:
    printf("Severity: notification");
    break;
  }
  printf("\n\n");
}

int main() {

  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);

  // glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
  // glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

  GLFWwindow *window = glfwCreateWindow(WIDTH, HEIGHT, TITLE, NULL, NULL);
  if (window == NULL) {
    fprintf(stdout, "Failed to make window!");
    glfwTerminate();
    return -1;
  }

  glfwMakeContextCurrent(window);
  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    printf("Failed to initialize GLAD");
    return -1;
  }
  // debug enabling
  int flags;
  glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
  if (flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(glDebugOutput, NULL);
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, NULL,
                          GL_TRUE);
  }

  glViewport(0, 0, WIDTH, HEIGHT);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

  ShaderProg shaderProgram;
  initShaderProgram(&shaderProgram);
  Shader vertexShader = {.type = GL_VERTEX_SHADER};
  Shader fragmentShader = {.type = GL_FRAGMENT_SHADER};
  initShaderFromFile(&vertexShader, "./shaders/vertex.glsl");
  initShaderFromFile(&fragmentShader, "./shaders/fragment.glsl");
  Shader shaders[] = {vertexShader, fragmentShader};
  attachShadersToProgram(&shaderProgram, shaders, 2);

  Texture texture;
  initTexture(&texture, "container", "./textures/container.jpg");

  Texture texture2;
  initTexture(&texture2, "face", "./textures/awesomeface.png");

  Texture textures[] = {texture, texture2};

  Mesh mesh;
  initMesh(&mesh, (Vertex *)vertices, (int)(sizeof(vertices) / sizeof(Vertex)),
           elements, LENGTH(elements));

  setTexturesMesh(&mesh, textures, 2);
  setShaderProgramMesh(&mesh, &shaderProgram);

  while (!glfwWindowShouldClose(window)) {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    useShader(&mesh);
    mat4 transform = GLM_MAT4_IDENTITY_INIT;
    vec4 translation = {0.0, -0.4, 0};
    mat4 rotated;
    vec3 axis = {0,0,1};
    glm_rotate(transform, (float)glfwGetTime(),axis );
    glm_translate(transform, translation);
    glUniform1f(glGetUniformLocation(shaderProgram.id, "t"),
                (float)glfwGetTime());
    glUniformMatrix4fv(glGetUniformLocation(shaderProgram.id, "transform"), 1,
                       GL_FALSE, transform[0]);
    drawMesh(&mesh);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  freeMesh(&mesh);

  freeShaderProgram(&shaderProgram);

  glfwTerminate();
  return 0;
}