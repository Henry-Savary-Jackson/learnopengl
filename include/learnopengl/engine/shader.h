
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
} ShaderProgram;


void initShaderProgram(ShaderProgram*shaderProg) ;
void initShaderProgramFromFiles(ShaderProgram* shaderProgram, const char * vsPath, const char * fsPath);

void initShaderFromFile(Shader* shader,const char *path) ;

void useShaderProgram(ShaderProgram* shaderProg);
void unbindShaderProgram();

void attachShadersToProgram(ShaderProgram* program, Shader shaders[], int length) ;

void freeShader(Shader* shader);
void freeShaderProgram(ShaderProgram* shader);
