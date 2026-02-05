

#include "learnopengl/engine/batch_render.h"
#include "learnopengl/engine/camera.h"
#include "learnopengl/engine/global_info.h"
#include "learnopengl/engine/object.h"
#include <learnopengl/engine/render_context.h>

void initRenderingContext(RenderingContext* context, Camera* camera, uint maxLights){
    initLightInfoManager(&context->lightManager, maxLights);
    initProjectionMatrixManager(&context->projectionManager);
    context->camera = camera;
}
void updateContext(RenderingContext* context){
    flushLightManager(&context->lightManager);
    setProjectionMatrixManager(&context->projectionManager,context->camera);
    flushProjectionMatrixManager(&context->projectionManager);
}

void addLightToContext(RenderingContext* context, LightSource* light ){
    mat4 view;
    getViewMatrix(context->camera, view);
    addLightToManager(&context->lightManager, light, view);
}

void attachContextToShader(RenderingContext* context, ShaderProgram* shaderProgram){
    bindProjectionManagerShader(&context->projectionManager, shaderProgram);
    bindLightInfoManager(&context->lightManager, shaderProgram);
}
void freeRenderingContext(RenderingContext* context){
    freeLightManager(&context->lightManager);
    freeProjectionManager(&context->projectionManager);
}
