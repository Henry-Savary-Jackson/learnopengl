#include "learnopengl/engine/batch_render.h"
#include "learnopengl/engine/camera.h"
#include "learnopengl/engine/global_info.h"
#include "learnopengl/engine/mesh.h"
#include "learnopengl/engine/object.h"
#include "learnopengl/engine/shader.h"
#include <learnopengl/utils/arraylist.h>
#include <learnopengl/utils/map.h>



typedef struct {
    ArrayList_p dynamicObjects; // pointers
    ArrayList_p staticObjects; // pointers
    ShaderProgram* shaderProgram;
} ShaderObjects;

#define MAX_SHADERS 20
#define STATIC_OBJ 0
#define DYNAMIC_OBJ 1


typedef struct{

    // list of dynamic  and static objects, referenced by Shader objects
    GameObject** gameObjects;
    LightSource** lights;
    // keep a fixed size array for simplicity
    // realisitically more than 20 different shader programs is absurd
    // index 0 should be a default shader if object has no shader attached.
    ShaderObjects* shaderObjects;

    LightInfoManager lightManager;
    ProjectionManager projectionManager;
    // camera that the user controls via user input
    Camera* camera;

    BatchRenderer dynamicRenderer;
    BatchRenderer staticRenderer;

    uint maxLights;
    uint maxTextures;
    uint maxObjects;
    uint maxShaders;

    uint numShaders;
    uint numLights;
    uint numObjects;

} Scene;

void initScene(Scene* scene,Camera* camera, uint maxShaders, uint maxObjects, uint maxLights);
void addObjectToScene(Scene* scene, GameObject* object, uint objectType);
void addLightToScene(Scene* scene, LightSource* light, uint objectType);


void preLoop(Scene* scene);

void renderLoop(Scene* scene);

void endLoop(Scene* scene);

void initShaderObjects(ShaderObjects *shaderObjects, ShaderProgram *program) ;

void addObjectToCorrectShaderList(Scene *scene, GameObject *object,
                                  uint objectType) ;
