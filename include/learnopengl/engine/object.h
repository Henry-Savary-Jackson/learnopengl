
#pragma once

#include <learnopengl/engine/mesh.h>
#include <learnopengl/engine/shader.h>
typedef struct {
    vec3s colour;
    alignas(16) float ambientCoeffient;
    float diffuseCoeffient;
    float specularCoefficient;
    alignas(16) float reflectionIndex;
    float refractionIndex;
    int specularMap;
    int normalMap;
} MaterialData;

typedef struct {
    uint id;
    vec3s colour;
    float ambientCoeffient;
    float diffuseCoeffient;
    float specularCoefficient;
    float reflectionIndex;
    float refractionIndex;

    Texture* specularMap;
    Sampler* specularSampler;

    Texture* normalMap;
    Sampler* normalSampler;
} Material;



typedef struct{
    vec3s rotation;
    vec3s position;
    vec3s scale;
    Mesh* mesh;
    ShaderProgram* shaderProgram;
    Texture* textures;
    Sampler* sampler; // each object with a texture can define properties
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
void setMaterialGameObject(GameObject* object, Material* material);
void setTexturesGameObject(GameObject* object, Texture* textures, int numTextures);
void setShaderProgramGameObject(GameObject* object, ShaderProgram* shaderProgram);
void setSampler(GameObject* object, Sampler* sampler);

void getModelMatrix(GameObject* object, mat4 dest);
void rotateGameObject(GameObject* object, vec3 rotation);
void translateGameObject(GameObject* object, vec3 translation);
void rotateGameObjectAround(GameObject* object, float angle, vec3 axis , vec3 pivot);
void scaleGameObject(GameObject* object, vec3 scale);
void scaleGameObjectFloat(GameObject* object, float scale);

void initMaterial(Material* material);
void setMaterialColour(Material* material, vec3s colour);
void setAmbientCoeffificent(Material* material, float ambient );
void setDiffuseCoeffificent(Material* material, float diffuse );
void setSpecularCoeffificent(Material* material, float specular );
void setReflectionCoeffificent(Material* material, float reflection  );
void setRefractionIndex(Material* material, float refractionIndex );
void setSpecularCoeffificent(Material* material, float specular );
void setNormalMap(Material* material, Texture* normalMap , Sampler* sampler);
void setSpecularMap(Material* material, Texture* specularMap , Sampler* sampler);
