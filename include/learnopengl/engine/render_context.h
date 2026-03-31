#pragma once

#include "learnopengl/engine/camera.h"
#include "learnopengl/engine/global_info.h"
#include "learnopengl/engine/object.h"
#include "learnopengl/engine/shader.h"
#include "learnopengl/engine/skybox.h"
typedef struct {
    ProjectionManager projectionManager;
    LightInfoManager lightManager;
    MaterialManager materialManager;
    TextureManager textureManager;
    SkyboxManager skybox;
    Camera* camera;
} RenderingContext ;
// add framebuffers to render to file/ screen/ sockets, etc...
void initRenderingContext(RenderingContext* context, Camera* camera, uint maxLights, uint maxMaterials, uint maxTextures);
void updateContext(RenderingContext* context);
void addLightToContext(RenderingContext* context, LightSource* light);
void attachContextToShader(RenderingContext* context, ShaderProgram* shaderProgram);
void addObjectToContext(RenderingContext* context, GameObject* object);
void freeRenderingContext(RenderingContext* context);
