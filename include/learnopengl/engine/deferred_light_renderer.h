

#include "learnopengl/engine/batch_render.h"
#include "learnopengl/engine/framebuffer.h"
#include "learnopengl/engine/light.h"
#include "learnopengl/engine/object.h"
#include "learnopengl/engine/scene.h"
#include "learnopengl/engine/shader.h"
#include <sys/types.h>

static const uint MAX_MESHES = 16;

#define ATTR_LOCATION_LIGHT_INDEX 16

typedef  struct{
    uint vao;
    uint vertexVBO;
    uint transformVBO;
    uint lightIndexVBO;
    uint vertexEBO;

    LightSource** lights;
    uint* meshIndices;
    uint numLights;
    uint numModels;
    uint maxLights;
    uint maxVertices;
    uint numVertices;
    uint numElements;

    LightData* lightDataBuffer;
    Vertex* vertexBuffer;
    mat4* transformBuffer;
    uint* lightIndexBuffer;
    uint* eboBuffer;

    Mesh* meshes;
    ShaderProgram* deferredLightingShader;


}DeferredLightRenderer;

typedef struct{
    Texture** gBufferTextures;
    FrameBuffer gBuffer;
    const char** gBufferTextureNames;
    BatchRenderer batchRenderer;
    DeferredLightRenderer lightVolumeRenderer;
    uint numGBufferTextures;
    ShaderProgram *shaderProgramGeom;
    ShaderProgram *shaderProgramLighting;
    Sampler sampler;
} DeferredRenderer;

void initDeferredRenderer(DeferredRenderer* renderer, const char ** gBufferTextureNames, uint numGBufferTextures, ShaderProgram* geom, ShaderProgram* light);
void renderDeferred(DeferredRenderer* renderer,Scene* scene, ShaderProgram* shaderProg);


void initDeferredLightRenderer(DeferredLightRenderer* deferredLightRenderer, uint maxLights );
void setLights(DeferredLightRenderer* deferredLightRenderer,LightSource** lightSource, uint numLights ,Mesh* mesh);
void renderAllLights(DeferredLightRenderer* renderer);
void freeDeferredLightRenderer(DeferredLightRenderer* deferredLightRenderer );
