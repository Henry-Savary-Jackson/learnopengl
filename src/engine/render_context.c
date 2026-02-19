#include "learnopengl/engine/camera.h"
#include "learnopengl/engine/global_info.h"
#include "learnopengl/engine/object.h"
#include <learnopengl/engine/render_context.h>

void initRenderingContext(RenderingContext* context, Camera* camera, uint maxLights, uint maxMaterials, uint maxTextures){
    initLightInfoManager(&context->lightManager, maxLights);
    initProjectionMatrixManager(&context->projectionManager);
    initMaterialManager(&context->materialManager, 100);
    initTextureManager(&context->textureManager, 200);
    context->camera = camera;
}
void updateContext(RenderingContext* context){
    flushLightManager(&context->lightManager);
    setProjectionMatrixManager(&context->projectionManager,context->camera);
    flushProjectionMatrixManager(&context->projectionManager);
    flushMaterialManager(&context->materialManager);
    flushTextureManager(&context->textureManager);
}

void addLightToContext(RenderingContext* context, LightSource* light ){
    mat4 view;
    getViewMatrix(context->camera, view);
    addLightToManager(&context->lightManager, light, view);
}
void addObjectToContext(RenderingContext *context, GameObject *object){
    addMaterialToManager(&context->materialManager, object->material, &context->textureManager);
}

void attachContextToShader(RenderingContext* context, ShaderProgram* shaderProgram){
    bindProjectionManagerShader(&context->projectionManager, shaderProgram);
    bindLightInfoManager(&context->lightManager, shaderProgram);
    bindMaterialManagerShader(&context->materialManager, shaderProgram);
    bindTextureManagerToShader(&context->textureManager, shaderProgram);
}
void freeRenderingContext(RenderingContext* context){
    freeLightManager(&context->lightManager);
    freeMaterialManager(&context->materialManager);
    freeProjectionManager(&context->projectionManager);
    freeTextureManager(&context->textureManager);
}
