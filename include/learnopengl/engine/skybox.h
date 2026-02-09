#pragma once
#include "learnopengl/engine/shader.h"
#include <sys/types.h>

typedef struct{
    uint vao;
    uint vertexVbo;
    uint textureIndex;
    float* vertexData;
    ShaderProgram* shaderProgram;
}Skybox;


void initSkybox(Skybox* skybox, ShaderProgram* shaderProgram);

void setSkyboxTextureIndex(Skybox *skybox, uint textureIndex);

void renderSkybox(Skybox* skybox);
