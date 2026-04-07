#include "learnopengl/engine/framebuffer.h"
#include <glad/glad.h>
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
#include <learnopengl/engine/skybox.h>
#include <learnopengl/engine/textures.h>
#include <learnopengl/engine/deferred_light_renderer.h>
#include <learnopengl/utils/arraylist.h>
#include <learnopengl/utils/utils.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <threads.h>

#define STB_IMAGE_IMPLEMENTATION
#include <learnopengl/utils/stb_image.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <learnopengl/utils/stb_image_write.h>

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
  setCameraDims(currentCam, width, height);
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

static uint framecount = 0;
float totalTimeSwap = 0;

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

  // ShaderProgram shaderProgram;
  // initShaderProgramFromFiles(&shaderProgram,
  //                            "./shaders/vertex_batched_with_lights.glsl",
  //                            "./shaders/fragment_lights.glsl");

  ShaderProgram lightShaderProgram;
  initShaderProgramFromFiles(&lightShaderProgram,
                             "./shaders/vertex_batched_for_lights.glsl",
                             "./shaders/light_source_fragment.glsl");

  ShaderProgram skyboxShader;
  initShaderProgramFromFiles(&skyboxShader,
                             "./shaders/vertex_skybox.glsl",
                             "./shaders/fragment_skybox.glsl");

  ShaderProgram gBufferShader;
  initShaderProgramFromFiles(&gBufferShader,
                             "./shaders/vertex_batched_with_lights.glsl","./shaders/fragment_deferred.glsl");

  ShaderProgram gBufferGeomShader;
    initShaderProgramFromFiles(&gBufferGeomShader,
      "./shaders/vertex_deferred_lighting.glsl","./shaders/fragment_deferred_geom_pass.glsl");


  ShaderProgram gBufferLightShader;
  initShaderProgramFromFiles(&gBufferLightShader,
                             "./shaders/vertex_deferred_lighting.glsl","./shaders/fragment_deferred_lighting.glsl");

  ShaderProgram gBufferRender;
  initShaderProgramFromFiles(&gBufferRender,
                             "./shaders/framebuffer_vs.glsl","./shaders/fragment_deferred_lighting_display_gbuffer.glsl");

  ShaderProgram hdrShader;
  initShaderProgramFromFiles(&hdrShader,
                             "./shaders/framebuffer_vs.glsl","./shaders/framebuffer_fs_combine.glsl");





  Texture texture;
  initTexture(&texture, "container", GL_TEXTURE_2D, GL_RGB, GL_RGB8, GL_UNSIGNED_BYTE);
  loadTexture2D(&texture, "./textures/container.jpg");

  Texture skyBoxTexture;
  const char *faces[6] = {"right.jpg",  "left.jpg",  "top.jpg",
                          "bottom.jpg", "front.jpg", "back.jpg"};

  initTexture(&skyBoxTexture, "skybox", GL_TEXTURE_CUBE_MAP, GL_RGB, GL_RGB8, GL_UNSIGNED_BYTE);
  loadTextureCubeMap(&skyBoxTexture, "./textures/skybox", faces);

  Texture normalMapBrickwall;
  initTexture(&normalMapBrickwall, "brick_normal", GL_TEXTURE_2D, GL_RGB,GL_RGB8, GL_UNSIGNED_BYTE );
  loadTexture2D(&normalMapBrickwall, "./textures/brickwall_normal.jpg");

  Sampler sampler;
  initSampler(&sampler);
  setSamplerParameter(&sampler, GL_TEXTURE_WRAP_S, GL_REPEAT);
  setSamplerParameter(&sampler, GL_TEXTURE_WRAP_T, GL_REPEAT);
  setSamplerParameter(&sampler, GL_TEXTURE_MIN_FILTER,
                      GL_NEAREST_MIPMAP_LINEAR);
  setSamplerParameter(&sampler, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  Material material;
  initMaterial(&material);
  setMaterialColour(&material, (vec3s){1, 1, 0});
  setAmbientCoeffificent(&material, 0.02);
  setDiffuseCoeffificent(&material, 100);
  setSpecularCoeffificent(&material, 100);
  setRefractionIndex(&material, 1 / 1.52);
  setReflectionStrength(&material, 0.6);
  setNormalMap(&material, &normalMapBrickwall, &sampler);

  // put vertices together

  Mesh meshCube;
  loadModelFromFile("models/cube.obj", &meshCube);

  Mesh sphere;
  loadModelFromFile("models/sphere.obj", &sphere);

  uint numLights = 2000;
  float bound = 40;

  uint maxObjectsLight = numLights + 1000;
  uint maxObjects = 5000;

  Camera camera;

  currentCam = &camera;
  initCamera(&camera, camera_pos, 90, WIDTH, HEIGHT);

  Scene scene;
  initScene(&scene, &camera, 20, maxObjects, maxObjectsLight);

  setSkybox(&scene, &skyBoxTexture, &sampler, &skyboxShader);

  LightSource lights[numLights];
  for (int i = 0; i < numLights; i++) {
    lights[i].colour =
        (vec3s){rand_float(0, 1), rand_float(0, 1), rand_float(0, 1)};
    lights[i].power = rand_float(1,3 );
    initGameObject(&lights[i].object);
    setMeshGameObject(&lights[i].object, &sphere);
    setMaterialGameObject(&lights[i].object, &material);
    lights[i].object.position =
        (vec3s){rand_float(-bound, bound), rand_float(-bound, bound),
                rand_float(-bound, bound)};
    setShaderProgramGameObject(&lights[i].object, &lightShaderProgram);
    addLightToScene(&scene, &lights[i], DYNAMIC_OBJ);
    setSampler(&lights[i].object, &sampler);
    scaleGameObjectFloat(&lights[i].object,
                         getLightRadius(&lights[i]));
  }

  Mesh bullMesh;
  loadModelFromFile("models/bull.obj", &bullMesh);


  srand(10);
  int numObjects = 100;
  GameObject *objects = malloc(numObjects * sizeof(GameObject));
  for (int i = 0; i < numObjects; i++) {
    initGameObject(&objects[i]);
    scaleGameObjectFloat(&objects[i],  0.01);
    setMeshGameObject(&objects[i],  &bullMesh );
    // scaleGameObjectFloat(&objects[i], 1);
    // setMeshGameObject(&objects[i], &meshCube);
    setShaderProgramGameObject(&objects[i], &gBufferShader);
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

  // Renderer newRender;

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);
  glDepthFunc(GL_LEQUAL);
  glCullFace(GL_BACK);
  glfwSwapInterval(0);


  const char* names [8]= {"gPos", "gAlbedo", "gNormal", "gTangent", "gBiTangent", "gMaterialIndex", "gNormalMap", "gSpecularMap" };

  DeferredRenderer deferredRenderer;
  initDeferredRenderer(&deferredRenderer, names, LENGTH(names),&gBufferGeomShader , &gBufferLightShader, maxObjectsLight);

  float lastFrameUpdate = 0;

  // bind the manager to send global shader info

  FrameBuffer hdrFrameBuffer;
  Texture textureHdr;
  Texture * textureHdr_p = &textureHdr;
  initTexture(textureHdr_p, "HDR", GL_TEXTURE_2D, GL_RGB, GL_RGB8, GL_UNSIGNED_INT);
  setTextureParameter(textureHdr_p, GL_TEXTURE_MAG_FILTER ,GL_NEAREST);
  setTextureParameter(textureHdr_p, GL_TEXTURE_MIN_FILTER ,GL_NEAREST);
  initFrameBuffer(&hdrFrameBuffer, 1 , &textureHdr_p);

  bool saved = false;
  preLoop(&scene);

  float dt = 0;
  while (!glfwWindowShouldClose(window)) {
    float startTime = glfwGetTime();
    uint width = camera.width;
    uint height = camera.height;
    glViewport(0, 0, width, height);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
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
    renderDeferred(&deferredRenderer, &scene, &gBufferRender);

    bindFrameBuffer(&hdrFrameBuffer,  width,  height);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
    glBlitFramebuffer(0,0,width,height, 0,0,width, height, GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT, GL_NEAREST );

    unbindFrameBuffer();
    const char * names[1] = {"scene"};
    setUniform1f(&hdrShader, "exposure", 1);
    renderFrameBuffer(&hdrFrameBuffer, width, height, &hdrShader, names );


    glfwPollEvents();
    glfwSwapBuffers(window);

    framecount++;

    // performance tracking
    dt = getDetlaTime();
    pollInput(window);
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
  freeShaderProgram(&gBufferShader);
  freeShaderProgram(&lightShaderProgram);
  freeMesh(&meshCube);
  freeMesh(&bullMesh);
  // freeMesh(&dragonMesh);
  freeSampler(&sampler);

  glfwTerminate();
  return 0;
}
