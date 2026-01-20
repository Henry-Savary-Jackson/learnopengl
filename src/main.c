#include <learnopengl/utils/arraylist.h>
#include <learnopengl/engine/loading/model_loader.h>
#include <learnopengl/engine/batch_render.h>
#include <learnopengl/engine/camera.h>
#include <learnopengl/engine/global_info.h>
#include <learnopengl/config.h>
#include <learnopengl/engine/mesh.h>
#include <learnopengl/engine/object.h>
#include <learnopengl/engine/render.h>
#include <learnopengl/engine/shader.h>
#include <learnopengl/engine/textures.h>
#include <learnopengl/utils/utils.h>
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
#include <learnopengl/utils/stb_image.h>

#define debug_impl
#include <learnopengl/utils/debug.h>

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
    0.0f,  0.0f,  -1.0f, 0.0f,  0.0f,  -1.0f, 0.0f,  0.0f,  -1.0f,
    0.0f,  0.0f,  -1.0f, 0.0f,  0.0f,  -1.0f, 0.0f,  0.0f,  -1.0f,

    0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,  .0f,   0.0f,  1.0f,
    0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,

    -1.0f, 0.0f,  0.0f,  -1.0f, 0.0f,  0.0f,  -1.0f, 0.0f,  0.0f,
    -1.0f, 0.0f,  0.0f,  -1.0f, 0.0f,  0.0f,  -1.0f, 0.0f,  0.0f,

    1.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
    1.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

    0.0f,  -1.0f, 0.0f,  0.0f,  -1.0f, 0.0f,  0.0f,  -1.0f, 0.0f,
    0.0f,  -1.0f, 0.0f,  0.0f,  -1.0f, 0.0f,  0.0f,  -1.0f, 0.0f,

    0.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
    0.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f};

float verticesUV[] = {
    -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, 0.5f,  -0.5f, -0.5f, 1.0f, 0.0f,
    0.5f,  0.5f,  -0.5f, 1.0f, 1.0f, 0.5f,  0.5f,  -0.5f, 1.0f, 1.0f,
    -0.5f, 0.5f,  -0.5f, 0.0f, 1.0f, -0.5f, -0.5f, -0.5f, 0.0f, 0.0f,

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
  initShaderFromFile(&vertexShader, "./shaders/vertex_batched_with_lights.glsl");
  initShaderFromFile(&fragmentShader, "./shaders/fragment_lights.glsl");
  Shader shaders[] = {vertexShader, fragmentShader};
  attachShadersToProgram(&shaderProgram, shaders, 2);

  ShaderProgram lightShaderProgram;
  initShaderProgram(&lightShaderProgram);
  Shader vertexShaderLight = {.type = GL_VERTEX_SHADER};
  Shader fragmentShaderLight = {.type = GL_FRAGMENT_SHADER};
  initShaderFromFile(&vertexShaderLight, "./shaders/vertex_batched_for_lights.glsl");
  initShaderFromFile(&fragmentShaderLight,
                     "./shaders/light_source_fragment.glsl");
  Shader shaderLight[] = {vertexShaderLight, fragmentShaderLight};
  attachShadersToProgram(&lightShaderProgram, shaderLight, 2);

  Texture texture;
  initTexture(&texture, "container", "./textures/container.jpg");

  Texture texture2;
  initTexture(&texture2, "face", "./textures/awesomeface.png");

  Texture textures[] = {texture, texture2};

  // put vertices together
  int numVertices = 36;
  Vertex vertexData[numVertices];
  for (int i = 0; i < numVertices; i++) {
    float *destPtr = (float *)vertexData;
    memcpy(destPtr + i * 8, verticesUV + i * 5, 20);
    int offset = 5;
    memcpy(destPtr + i * 8 + offset, verticesNorms + i * 3, 12);
  }
  float bound =100;

  Mesh mesh;
  initMesh(&mesh, vertexData, LENGTH(vertexData), elements, LENGTH(elements));

  uint numLights = 5;

  LightSource lights [numLights];
  for (int i = 0 ; i < numLights ; i++){

      lights[i].colour = (vec3s){rand_float(0, 1), rand_float(0, 1), rand_float(0, 1)};
      lights[i].power = rand_float(10, 50);
      initGameObject(&lights[i].object);
      setMeshGameObject(&lights[i].object, &mesh);
      lights[i].object.position = (vec3s){rand_float(-bound, bound), rand_float(-bound, bound), rand_float(-bound, bound)};
      // translateGameObject(&lights[i].object, (vec3){1,3,2});
      // setShaderProgramGameObject(&lights[i].object, &lightShaderProgram);
  }

  Camera camera;
  currentCam = &camera;
  initCamera(&camera, camera_pos, 90, WIDTH, HEIGHT);

  uint maxObjects = 50000;
  uint maxVertices = 1800000;

  ArrayList_i elementIndex;
  initArrayList_i(&elementIndex,  1000, 1000);
  ArrayList_vertex vertices;
  initArrayList_vertex(&vertices,  1000, 1000);
  loadModelFromFile("models/bull.obj", &vertices , &elementIndex);

  Mesh bullMesh ;
  initMesh(&bullMesh, vertices.ptr, vertices.length, elementIndex.ptr, elementIndex.length);

  Material material = {.ambientCoeffient = 1,
                       .diffuseCoeffient = 0.2,
                       .specularCoefficient = 0.4,
                       .colour = {1,0,0}};

  MaterialManager materialManager;
  initMaterialManager(&materialManager);
  setMaterialManagerMaterial(&materialManager, &material);


  uint maxObjectsLight = numLights+ 1000;
  LightInfoManager lightManager;
  initLightInfoManager(&lightManager, maxObjectsLight);

  TransformationMatrixManager transformManager;
  initTransformMatrixManager(&transformManager, maxObjects);

  ProjectionManager projectionManager;
  initProjectionMatrixManager(&projectionManager);

  BatchRenderer renderer;

  uint maxVerticesLight = 36 * maxObjectsLight;
  ;

  initBatchRenderer(&renderer, &transformManager ,
                     maxVertices);

  BatchRenderer lightRenderer;

  initBatchRenderer(&lightRenderer, &transformManager ,
                     maxVerticesLight);

  srand(10);
  int numObjects = 6;
  GameObject *objects = malloc(numObjects * sizeof(GameObject));
  for (int i = 0; i < numObjects; i++) {
    initGameObject(&objects[i]);
    setMeshGameObject(&objects[i], &bullMesh);
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
    scaleGameObjectFloat(&objects[i], 0.06);
    rotateGameObject(&objects[i], rotation_cube.raw);
    translateGameObject(&objects[i], translate_cube.raw);
  }
  objects[0].position = (vec3s){0, 0, -2};
  objects[0].rotation = (vec3s){0, 0, 0};
  // Renderer newRender;
  glEnable(GL_DEPTH_TEST);

  float lastFrameUpdate = 0;

  // bind the manager to send global shader info
  bindLightInfoManager(&lightManager,  &shaderProgram);

  setMaterialManagerMaterial(&materialManager, &material);
  bindMaterialManagerShader(&materialManager, &shaderProgram);
  flushMaterialManager(&materialManager);

  bindProjectionManagerShader(&projectionManager, &shaderProgram);

  bindTexturesUniform(&shaderProgram, textures, 2);
  bindTransformMatrixManager(&transformManager, &shaderProgram);

  bindProjectionManagerShader( &projectionManager,&lightShaderProgram);
  // useShaderProgram(&lightShaderProgram);
  // glUniform3f(glGetUniformLocation(lightShaderProgram.id, "lightColour"), lights[0].colour.r, lights[0].colour.g, lights[0].colour.b);
  // unbindShaderProgram();

  bindTransformMatrixManager(&transformManager, &lightShaderProgram);

  // setProjectionMatrixManager(&projectionManager, &camera);
  while (!glfwWindowShouldClose(window)) {
    float startTime = glfwGetTime();
    glClearColor(0.2f, 0.4f, 0.4f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // add projection matrices
    setProjectionMatrixManager(&projectionManager, &camera);

    addLightsToManager(&lightManager, lights, numLights, projectionManager.projectionBuffer->viewMatrix);
    flushLightManager(&lightManager);
    // send GPU
    flushProjectionMatrixManager(&projectionManager);
     // transform manager is flushed by the batched renderer
    useShaderProgram(&shaderProgram);

    for (int i = 0; i < numObjects; i++) {
      renderObjectBatch(&renderer, &objects[i]);
    }

    flush(&renderer);

    // add light information
    useShaderProgram(&lightShaderProgram);
    flushTransformationManager(&transformManager);

    for (int i = 0 ; i < numLights; i ++){
      float dt = getDetlaTime();
      rotateGameObjectAround(&(lights[i].object), 180.f * dt,
                           (vec3){0, 2, 1}, (vec3){4, 2, 3});
      renderObjectBatch(&renderer, &(lights[i].object));
    }
    flush(&renderer);

    unbindShaderProgram();

    glfwSwapBuffers(window);
    glfwPollEvents();
    pollInput(window);

    // performance tracking
    lastTime = glfwGetTime();

    if (lastTime - lastFrameUpdate > 0.1) {
      lastFrameUpdate = lastTime;
      float frameTime = (lastTime - startTime);
      char frameTimeStr[strlen("Time per frame: , Frame rate: ") + 15];
      sprintf(frameTimeStr, "Time per frame: %.5f , Frame rate: %d", frameTime,
              (int)(1 / frameTime));
      glfwSetWindowTitle(window, frameTimeStr);
    }
  }

  free(objects);
  freeTexture(&texture);
  freeLightManager(&lightManager);
  freeMaterialManager(&materialManager);
  freeTransformMatrixManager(&transformManager);
  freeProjectionManager(&projectionManager);
  freeBatchRenderer(&renderer);
  freeBatchRenderer(&lightRenderer);
  freeShaderProgram(&shaderProgram);
  freeShaderProgram(&lightShaderProgram);
  // freeRenderer(&lightRenderer);

  glfwTerminate();
  return 0;
}
