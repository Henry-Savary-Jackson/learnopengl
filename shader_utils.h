
#pragma  once
#include <sys/types.h>

typedef struct{
    uint id;
    uint type;
} Shader;

typedef struct{
    uint id;
    Shader* shaders;
    uint num_shaders;
} ShaderProg;


void initShaderProgram(ShaderProg*shaderProg) ; 

void initShaderFromFile(Shader* shader,const char *path) ;

void useShaderProgram(ShaderProg* shaderProg);

void attachShadersToProgram(ShaderProg* program, Shader shaders[], int length) ;

void freeShader(Shader* shader);
void freeShaderProgram(ShaderProg* shader);
