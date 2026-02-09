#include <glad/glad.h>
#include <learnopengl/engine/skybox.h>
#include <GLFW/glfw3.h>
#include <cglm/cam.h>
#include <cglm/mat4.h>
#include <cglm/vec3.h>
#include <learnopengl/config.h>
#include <learnopengl/engine/batch_render.h>
#include <learnopengl/engine/camera.h>
#include <learnopengl/engine/global_info.h>
#include <learnopengl/engine/loading/model_loader.h>
#include <learnopengl/engine/mesh.h>
#include <learnopengl/engine/object.h>
#include <learnopengl/engine/scene.h>
#include <learnopengl/engine/shader.h>
#include <learnopengl/engine/textures.h>
#include <learnopengl/utils/arraylist.h>
#include <learnopengl/utils/utils.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STB_IMAGE_IMPLEMENTATION
#include <learnopengl/utils/stb_image.h>

#define debug_impl
#include <learnopengl/utils/debug.h>

void enableDebugOutput();
const char *title_base = "Time per frame: , Frame rate: ";

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

float getDetlaTime() { return glfwGetTime() - lastTime; }
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
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
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
  initShaderProgramFromFiles(&shaderProgram, "./shaders/vertex_batched_with_lights.glsl", "./shaders/fragment_lights.glsl");

  ShaderProgram lightShaderProgram;
  initShaderProgramFromFiles(&lightShaderProgram, "./shaders/vertex_batched_for_lights.glsl",  "./shaders/light_source_fragment.glsl");

  ShaderProgram skyboxShader;
  initShaderProgramFromFiles(&skyboxShader, "./shaders/vertex_skybox.glsl",  "./shaders/fragment_skybox.glsl");

  Texture texture;
  initTexture(&texture, "container");
  loadTexture2D(&texture, "./textures/container.jpg");

  Texture skyBoxTexture;
  const char *faces[6] = {"right.jpg",  "left.jpg",  "top.jpg",
                          "bottom.jpg", "front.jpg", "back.jpg"};

  initTexture(&skyBoxTexture, "skybox");
  loadTextureCubeMap(&skyBoxTexture, "./textures/skybox", faces);

  Skybox skybox;
  initSkybox(&skybox, &skyboxShader);


  Material material;
  initMaterial(&material, (vec3s){1, 1, 0}, 4, 1, 1);

  Sampler sampler;
  initSampler(&sampler);
  setSamplerParameter(&sampler, GL_TEXTURE_WRAP_S, GL_REPEAT);
  setSamplerParameter(&sampler, GL_TEXTURE_WRAP_T, GL_REPEAT);
  setSamplerParameter(&sampler, GL_TEXTURE_MIN_FILTER,
                      GL_NEAREST_MIPMAP_LINEAR);
  setSamplerParameter(&sampler, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  // put vertices together

  Mesh meshCube;
  loadModelFromFile("models/cube.obj", &meshCube);

  uint numLights = 15;
  float bound = 20;

  uint maxObjectsLight = numLights + 1000;
  uint maxObjects = 5000;

  Camera camera;

  currentCam = &camera;
  initCamera(&camera, camera_pos, 90, WIDTH, HEIGHT);

  Scene scene;
  initScene(&scene, &camera, 20, maxObjects, maxObjectsLight);

  LightSource lights[numLights];
  for (int i = 0; i < numLights; i++) {
    lights[i].colour =
        (vec3s){rand_float(0, 1), rand_float(0, 1), rand_float(0, 1)};
    lights[i].power = rand_float(500, 700);
    initGameObject(&lights[i].object);
    setMeshGameObject(&lights[i].object, &meshCube);
    setMaterialGameObject(&lights[i].object, &material);
    lights[i].object.position =
        (vec3s){rand_float(-bound, bound), rand_float(-bound, bound),
                rand_float(-bound, bound)};
    setShaderProgramGameObject(&lights[i].object, &lightShaderProgram);
    addLightToScene(&scene, &lights[i], DYNAMIC_OBJ);
    setSampler(&lights[i].object, &sampler);
  }

  Mesh bullMesh;
  loadModelFromFile("models/bull.obj", &bullMesh);

  Mesh dragonMesh;
  loadModelFromFile("models/bunny.obj", &dragonMesh);

  srand(10);
  int numObjects = 100;
  GameObject *objects = malloc(numObjects * sizeof(GameObject));
  for (int i = 0; i < numObjects; i++) {
    initGameObject(&objects[i]);
    scaleGameObjectFloat(&objects[i], i % 2 ? 0.01 : 10);
    setMeshGameObject(&objects[i], i % 2 ? &bullMesh : &dragonMesh);
    setShaderProgramGameObject(&objects[i], &shaderProgram);
    setTexturesGameObject(objects + i, &texture, 1);
    setMaterialGameObject(&objects[i], &material);

    vec3s rotation_cube = {
        rand_float(-70, 180),
        rand_float(-90, 180),
        rand_float(-200, 180),
    };
    vec3s translate_cube = {rand_float(-bound, bound),
                            rand_float(-bound, bound),
                            rand_float(-bound, bound)};
    // scaleGameObjectFloat(&objects[i], 0.06);
    rotateGameObject(&objects[i], rotation_cube.raw);
    translateGameObject(&objects[i], translate_cube.raw);

    setSampler(&objects[i], &sampler);

    addObjectToScene(&scene, &objects[i], DYNAMIC_OBJ);
  }
  objects[0].position = (vec3s){0, 0, -2};
  objects[0].rotation = (vec3s){0, 30, 0};

  Sampler sampler2;
  initSampler(&sampler2);
  setSamplerParameter(&sampler2, GL_TEXTURE_WRAP_S, GL_REPEAT);
  setSamplerParameter(&sampler2, GL_TEXTURE_WRAP_T, GL_REPEAT);
  setSamplerParameter(&sampler2, GL_TEXTURE_MIN_FILTER,
                      GL_NEAREST_MIPMAP_LINEAR);
  setSamplerParameter(&sampler2, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  setSkybox(&scene, &skybox, &skyBoxTexture, &sampler2);
  // Renderer newRender;

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glCullFace(GL_BACK);
  glfwSwapInterval(0);

  float lastFrameUpdate = 0;

  // bind the manager to send global shader info

  preLoop(&scene);

  float dt = 0;
  while (!glfwWindowShouldClose(window)) {
    float startTime = glfwGetTime();
    glClearColor(0.2f, 0.4f, 0.4f, 1.0f);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // -- GAME LOGIC --

    for (int i = 0; i < numObjects; i++) {
      glm_vec3_add(objects[i].rotation.raw, (vec3){1 * dt, 1 * dt, 0.4 * dt},
                   objects[i].rotation.raw);
    }

    for (int i = 0; i < numLights; i++) {
      rotateGameObjectAround(&(lights[i].object), 1.f * dt, (vec3){0, 2, 1},
                             (vec3){4, 2, 3});
    }

    // -- CALL TO RENDER --
    renderLoop(&scene);

    glfwSwapBuffers(window);
    glfwPollEvents();
    pollInput(window);

    // performance tracking
    dt = getDetlaTime();
    lastTime = glfwGetTime();

    if (lastTime - lastFrameUpdate > 0.1) {
      lastFrameUpdate = lastTime;
      float frameTime = (lastTime - startTime);
      char frameTimeStr[strlen(title_base) + 15];
      sprintf(frameTimeStr, "Time per frame: %.5f , Frame rate: %d", frameTime,
              (int)(1 / frameTime));
      glfwSetWindowTitle(window, frameTimeStr);
    }
    // printf("frame done!\n");
  }

  freeScene(&scene);
  free(objects);
  freeTexture(&texture);
  freeShaderProgram(&shaderProgram);
  freeShaderProgram(&lightShaderProgram);
  freeMesh(&meshCube);
  freeMesh(&bullMesh);
  freeMesh(&dragonMesh);
  freeSampler(&sampler);

  glfwTerminate();
  return 0;
}
