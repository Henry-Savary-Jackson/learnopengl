
#pragma once

#include <learnopengl/engine/mesh.h>
#include <learnopengl/engine/shader.h>
typedef struct {
    float ambientCoeffient;
    float diffuseCoeffient;
    float specularCoefficient;
    alignas(16 )vec3s colour;
} Material;



typedef struct{
    vec3s rotation;
    vec3s position;
    vec3s scale;
    Mesh* mesh;
    ShaderProgram* shaderProgram;
    Texture* textures;
    uint numTextures;
    Material* material;
} GameObject;

typedef struct {
    GameObject object;
    vec3s colour;
    float power;
} LightSource;



void initGameObject(GameObject* object);
void setMeshGameObject(GameObject* object, Mesh* mesh);
void setTexturesGameObject(GameObject* object, Texture* textures, int numTextures);
void setShaderProgramGameObject(GameObject* object, ShaderProgram* shaderProgram);
void getModelMatrix(GameObject* object, mat4 dest);
void rotateGameObject(GameObject* object, vec3 rotation);
void translateGameObject(GameObject* object, vec3 translation);
void rotateGameObjectAround(GameObject* object, float angle, vec3 axis , vec3 pivot);
void scaleGameObject(GameObject* object, vec3 scale);
void scaleGameObjectFloat(GameObject* object, float scale);
