
#pragma once

#include "mesh.h"
#include "shader.h"
typedef struct{
    vec3s rotation;
    vec3s position;
    Mesh* mesh;
    ShaderProgram* shaderProgram;
    Texture* textures;
    uint numTextures;
} GameObject;

void initGameObject(GameObject* object);
void setMeshGameObject(GameObject* object, Mesh* mesh);
void setTexturesGameObject(GameObject* object, Texture* textures, int numTextures);
void setShaderProgramGameObject(GameObject* object, ShaderProgram* shaderProgram);
void getModelMatrix(GameObject* object, mat4 dest);

void rotateGameObject(GameObject* object, vec3 rotation);
void translateGameObject(GameObject* object, vec3 translation);


