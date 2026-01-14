#include "batch_render.h"
#include "camera.h"
#include "config.h"
#include "mesh.h"
#include "object.h"
#include "render.h"
#include "shader.h"
#include "textures.h"
#include "utils.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cglm/affine-post.h>
#include <cglm/cam.h>
#include <cglm/mat4.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define debug_impl
#include "debug.h"

void enableDebugOutput();

float vertices[] = {
    // positions          // texture coords
    0.5f,  0.5f,  0.5f,  1.0f, 1.0f, -0.5f, 0.5f,  0.5f,  .0f,  1.0f,
    -0.5f, -0.5f, 0.5f,  .0f,  .0f,  0.5f,  -0.5f, 0.5f,  1.0f, .0f,
    0.5f,  0.5f,  -0.5f, .0f,  1.0f, -0.5f, 0.5f,  -0.5f, 1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, 1.0f, .0f,  0.5f,  -0.5f, -0.5f, .0f,  .0f};

float verticesFull[] = {
    -0.5f, -0.5f, -0.5f, 0.0f,  0.0f,  -1.0f, 0.5f,  -0.5f, -0.5f,
    0.0f,  0.0f,  -1.0f, 0.5f,  0.5f,  -0.5f, 0.0f,  0.0f,  -1.0f,
    0.5f,  0.5f,  -0.5f, 0.0f,  0.0f,  -1.0f, -0.5f, 0.5f,  -0.5f,
    0.0f,  0.0f,  -1.0f, -0.5f, -0.5f, -0.5f, 0.0f,  0.0f,  -1.0f,

    -0.5f, -0.5f, 0.5f,  0.0f,  0.0f,  1.0f,  0.5f,  -0.5f, 0.5f,
    0.0f,  0.0f,  1.0f,  0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
    0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  -0.5f, 0.5f,  0.5f,
    0.0f,  0.0f,  1.0f,  -0.5f, -0.5f, 0.5f,  0.0f,  0.0f,  1.0f,

    -0.5f, 0.5f,  0.5f,  -1.0f, 0.0f,  0.0f,  -0.5f, 0.5f,  -0.5f,
    -1.0f, 0.0f,  0.0f,  -0.5f, -0.5f, -0.5f, -1.0f, 0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f, -1.0f, 0.0f,  0.0f,  -0.5f, -0.5f, 0.5f,
    -1.0f, 0.0f,  0.0f,  -0.5f, 0.5f,  0.5f,  -1.0f, 0.0f,  0.0f,

    0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.5f,  0.5f,  -0.5f,
    1.0f,  0.0f,  0.0f,  0.5f,  -0.5f, -0.5f, 1.0f,  0.0f,  0.0f,
    0.5f,  -0.5f, -0.5f, 1.0f,  0.0f,  0.0f,  0.5f,  -0.5f, 0.5f,
    1.0f,  0.0f,  0.0f,  0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

    -0.5f, -0.5f, -0.5f, 0.0f,  -1.0f, 0.0f,  0.5f,  -0.5f, -0.5f,
    0.0f,  -1.0f, 0.0f,  0.5f,  -0.5f, 0.5f,  0.0f,  -1.0f, 0.0f,
    0.5f,  -0.5f, 0.5f,  0.0f,  -1.0f, 0.0f,  -0.5f, -0.5f, 0.5f,
    0.0f,  -1.0f, 0.0f,  -0.5f, -0.5f, -0.5f, 0.0f,  -1.0f, 0.0f,

    -0.5f, 0.5f,  -0.5f, 0.0f,  1.0f,  0.0f,  0.5f,  0.5f,  -0.5f,
    0.0f,  1.0f,  0.0f,  0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
    0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  -0.5f, 0.5f,  0.5f,
    0.0f,  1.0f,  0.0f,  -0.5f, 0.5f,  -0.5f, 0.0f,  1.0f,  0.0f};

    float verticesNorms[] = {
          0.0f,  0.0f, -1.0f,
          0.0f,  0.0f, -1.0f,
          0.0f,  0.0f, -1.0f,
          0.0f,  0.0f, -1.0f,
          0.0f,  0.0f, -1.0f,
          0.0f,  0.0f, -1.0f,

          0.0f,  0.0f,  1.0f,
          0.0f,  0.0f,  1.0f,
        .0f,  0.0f,  1.0f,
          0.0f,  0.0f,  1.0f,
          0.0f,  0.0f,  1.0f,
          0.0f,  0.0f,  1.0f,

         -1.0f,  0.0f,  0.0f,
         -1.0f,  0.0f,  0.0f,
         -1.0f,  0.0f,  0.0f,
         -1.0f,  0.0f,  0.0f,
         -1.0f,  0.0f,  0.0f,
        -1.0f,  0.0f,  0.0f,

          1.0f,  0.0f,  0.0f,
          1.0f,  0.0f,  0.0f,
          1.0f,  0.0f,  0.0f,
          1.0f,  0.0f,  0.0f,
          1.0f,  0.0f,  0.0f,
          1.0f,  0.0f,  0.0f,

          0.0f, -1.0f,  0.0f,
          0.0f, -1.0f,  0.0f,
         0.0f, -1.0f,  0.0f,
         0.0f, -1.0f,  0.0f,
         0.0f, -1.0f,  0.0f,
          0.0f, -1.0f,  0.0f,

         0.0f,  1.0f,  0.0f,
         0.0f,  1.0f,  0.0f,
          0.0f,  1.0f,  0.0f,
          0.0f,  1.0f,  0.0f,
          0.0f,  1.0f,  0.0f,
         0.0f,  1.0f,  0.0f
    };

float verticesUV[] = {
    -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,
     0.5f,  -0.5f, -0.5f, 1.0f, 0.0f,
    0.5f,  0.5f,  -0.5f, 1.0f, 1.0f,
     0.5f,  0.5f,  -0.5f, 1.0f, 1.0f,
    -0.5f, 0.5f,  -0.5f, 0.0f, 1.0f,
     -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,

    -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, 0.5f,  -0.5f, 0.5f,  1.0f, 0.0f,
    0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    -0.5f, 0.5f,  0.5f,  0.0f, 1.0f, -0.5f, -0.5f, 0.5f,  0.0f, 0.0f,

    -0.5f, 0.5f,  0.5f,  1.0f, 0.0f, -0.5f, 0.5f,  -0.5f, 1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,
    -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, -0.5f, 0.5f,  0.5f,  1.0f, 0.0f,

    0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.5f,  0.5f,  -0.5f, 1.0f, 1.0f,
    0.5f,  -0.5f, -0.5f, 0.0f, 1.0f, 0.5f,  -0.5f, -0.5f, 0.0f, 1.0f,
    0.5f,  -0.5f, 0.5f,  0.0f, 0.0f, 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f, 0.0f, 1.0f, 0.5f,  -0.5f, -0.5f, 1.0f, 1.0f,
    0.5f,  -0.5f, 0.5f,  1.0f, 0.0f, 0.5f,  -0.5f, 0.5f,  1.0f, 0.0f,
    -0.5f, -0.5f, 0.5f,  0.0f, 0.0f, -0.5f, -0.5f, -0.5f, 0.0f, 1.0f,

    -0.5f, 0.5f,  -0.5f, 0.0f, 1.0f, 0.5f,  0.5f,  -0.5f, 1.0f, 1.0f,
    0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f, 0.5f,  0.5f,  0.0f, 0.0f, -0.5f, 0.5f,  -0.5f, 0.0f, 1.0f};

uint elements[] = {0, 1, 2, 0, 2, 3, 2, 6, 7, 2, 7, 3, 0, 7, 3, 0, 4, 7,
                   1, 4, 0, 1, 5, 4, 5, 4, 7, 4, 6, 7, 1, 5, 6, 2, 1, 6};

Camera *currentCam = NULL;
vec3s camera_pos = {0, 0, 0};
vec2s camera_angle = {-90, 0};
float lastX = 400, lastY = 300;

void mouse_callback(GLFWwindow *window, double xpos, double ypos) {

  float xoffset = xpos - lastX;
  float yoffset =
      lastY - ypos; // reversed since y-coordinates range from bottom to top
  lastX = xpos;
  lastY = ypos;

  const float sensitivity = 0.1f;
  xoffset *= sensitivity;
  yoffset *= sensitivity;
  camera_angle.x += xoffset;
  camera_angle.y += yoffset;

  camera_angle.y = glm_max(glm_min(camera_angle.y, 70.f), -70.f);

  setCameraAngles(currentCam, camera_angle);
}

int x = 0;
int y = 0;

const float cameraSpeed = 3.f; // adjust accordingly
float lastTime = 0.f;

float getDetlaTime() {
  return glfwGetTime()  -lastTime;
}
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
  glViewport(0, 0, width, height);
  if (currentCam) {
    // setup camera
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    setCameraDims(currentCam, width, height);
  }
}
void pollInput(GLFWwindow *window) {
  float deltaTime = getDetlaTime();

  if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
    moveCameraFront(currentCam, cameraSpeed * deltaTime);
  }
  if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
    moveCameraFront(currentCam, -cameraSpeed * deltaTime);
  }
  if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
    moveCameraSide(currentCam, -cameraSpeed * deltaTime);
  }
  if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {

    moveCameraSide(currentCam, cameraSpeed * deltaTime);
  }
  if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
    moveCameraVertical(currentCam, cameraSpeed * deltaTime);
  }
  if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
    moveCameraVertical(currentCam, -cameraSpeed * deltaTime);
  }
}

float rand_float(float min, float max) {
  return min + (max - min) * ((float)random() / (float)RAND_MAX);
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

  enableDebugOutput();

  glViewport(0, 0, WIDTH, HEIGHT);
  glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
  glfwSetCursorPosCallback(window, mouse_callback);

  ShaderProgram shaderProgram;
  initShaderProgram(&shaderProgram);
  Shader vertexShader = {.type = GL_VERTEX_SHADER};
  Shader fragmentShader = {.type = GL_FRAGMENT_SHADER};
  initShaderFromFile(&vertexShader, "./shaders/vertex_batched_lights.glsl");
  initShaderFromFile(&fragmentShader, "./shaders/fragment.glsl");
  Shader shaders[] = {vertexShader, fragmentShader};
  attachShadersToProgram(&shaderProgram, shaders, 2);


  ShaderProgram lightShader;
  initShaderProgram(&lightShader);
  Shader vertexShaderLight = {.type = GL_VERTEX_SHADER};
  Shader fragmentShaderLight = {.type = GL_FRAGMENT_SHADER};
  initShaderFromFile(&vertexShaderLight, "./shaders/vertex.glsl");
  initShaderFromFile(&fragmentShaderLight, "./shaders/light_source_fragment.glsl");
  Shader shaderLight[] = {vertexShaderLight, fragmentShaderLight};
  attachShadersToProgram(&lightShader, shaderLight, 2);


  Texture texture;
  initTexture(&texture, "container", "./textures/container.jpg");

  Texture texture2;
  initTexture(&texture2, "face", "./textures/awesomeface.png");

  Texture textures[] = {texture, texture2};

  // put vertices together
  int numVertices  =36;
  Vertex vertexData [numVertices];
  for (int i =0; i < numVertices; i++) {
    float* destPtr = (float*)vertexData;
    memcpy(destPtr+i*8, verticesUV+i*5, 20);
    int offset = 5;
    memcpy(destPtr+i*8+offset, verticesNorms+i*3, 12);
  }

  Mesh mesh;
  initMesh(&mesh, vertexData, LENGTH(vertexData),
           elements, LENGTH(elements));

  LightSource light = {.position=(vec3s){5,2,1}, 
  .colour=(vec3s){1,0,0.1}, 
  

  .intensity=100,.pad_1=1000, .pad_2=1000};

  LightObject lightCube = {.light=&light};
  initGameObject(&lightCube.object);
  setMeshGameObject(&lightCube.object, &mesh);
  translateGameObject(&lightCube.object, light.position.raw);
  setShaderProgramGameObject(&lightCube.object, &lightShader);

  Camera camera;
  currentCam = &camera;
  initCamera(&camera, camera_pos, 90, WIDTH, HEIGHT);

  uint maxObjects = 10;
  uint maxVertices = 36000;

  uint* matIndexBuffer = malloc(maxVertices*sizeof(uint)) ;
  mat4* matBuffer  = malloc(maxObjects*sizeof(mat4)) ;;
  Vertex* vertexBuffer = malloc(maxVertices*sizeof(Vertex));


  Material material = {.ambientCoeffient = 0.01, .diffuseCoeffient=1, .specularCoefficient=0.8, .colour={0,10,0}};

  BatchRenderer renderer;

  initBatchRenderer(&renderer, &camera, &shaderProgram, vertexBuffer, matIndexBuffer, matBuffer, maxVertices, maxObjects);
  setTexutres(&renderer, textures, 2);
  setMaterial(&renderer, &material);
  setLight(&renderer, &light);


  Renderer lightRenderer;
  initRenderer(&lightRenderer, &camera);
  

  srand(10);
  float bound = 10;
  int numObjects = 200;
  GameObject* objects = malloc(numObjects*sizeof(GameObject));
  for (int i = 0; i < numObjects; i++) {
    initGameObject(&objects[i]);
    setMeshGameObject(&objects[i], &mesh);
    setShaderProgramGameObject(&objects[i], &shaderProgram);
    setTexturesGameObject(objects + i, textures, 2);

    vec3s rotation_cube = {
        rand_float(-70, 180),
        rand_float(-90, 180),
        rand_float(-200, 180),
    };
    vec3s translate_cube = {rand_float(-bound, bound),
                            rand_float(-bound, bound),
                            rand_float(-bound, bound)};
    rotateGameObject(&objects[i], rotation_cube.raw);
    translateGameObject(&objects[i], translate_cube.raw);
  }
  objects[0].position = (vec3s){0,0,-2};
  objects[0].rotation = (vec3s){0,0,0};
  // Renderer newRender;
  glEnable(GL_DEPTH_TEST);

  float lastFrameUpdate = 0;
  while (!glfwWindowShouldClose(window)) {
    float startTime = glfwGetTime();
    glClearColor(0.2f, 0.4f, 0.4f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float dt = getDetlaTime();

    for (int i = 0; i < numObjects; i++) {
      renderObjectBatch(&renderer, &objects[i]);
      // renderObject(&newRender, objects+i);
    }
    flush(&renderer);
    renderObject(&lightRenderer, &(lightCube.object));

    glfwSwapBuffers(window);
    glfwPollEvents();
    pollInput(window );
    lastTime = glfwGetTime();

    if (lastTime - lastFrameUpdate > 0.1) {
      lastFrameUpdate = lastTime;
      float frameTime = (lastTime - startTime);
      char frameTimeStr[strlen("Time per frame: , Frame rate: ") + 15];
      sprintf(frameTimeStr, "Time per frame: %.5f , Frame rate: %d", frameTime, (int)(1/frameTime));
      glfwSetWindowTitle(window, frameTimeStr);
    }
  }

  free(vertexBuffer);
  free(objects);
  free(matBuffer);
  free(matIndexBuffer);
  freeShaderProgram(&shaderProgram);
  freeRenderer(&lightRenderer);

  glfwTerminate();
  return 0;
}