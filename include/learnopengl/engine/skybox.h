#pragma once
#include "learnopengl/engine/shader.h"
#include <sys/types.h>

typedef struct{
    uint vao;
    uint vertexVbo;
    uint textureIndex;
    float* vertexData;
    ShaderProgram* shaderProgram;
}SkyboxManager;


void initSkybox(SkyboxManager* skybox);

void setSkyboxTextureIndex(SkyboxManager *skybox, uint textureIndex);

void renderSkybox(SkyboxManager* skybox);

void setSkyboxShader(SkyboxManager* skybox, ShaderProgram* shaderProgram);

void bindSkyboxManagerToShader(SkyboxManager* skybox, ShaderProgram* shaderProgram);
