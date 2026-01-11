
#include "mesh.h"
#include "shader_utils.h"
typedef struct{
    vec3s rotation;
    vec3s postion;
    Mesh* mesh;
    Texture* textures;
    ShaderProg* shaderProgram;
} GameObject;