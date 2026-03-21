
#pragma once
#include <sys/types.h>

typedef struct {
  uint id;
  uint type;
} Shader;

typedef struct {
  uint id;
  Shader *shaders;
  uint num_shaders;
} ShaderProgram;

#define ATTR_NAME_POS "aPos"
#define ATTR_NAME_UV "aTexCoord"
#define ATTR_NAME_NORM "aNormal"
#define ATTR_NAME_TANGENT "aTangent"
#define ATTR_NAME_BITANGENT "aBiTangent"
#define ATTR_NAME_MODEL "aModel"
#define ATTR_NAME_INVMODEL "aInvModel"
#define ATTR_NAME_MATERIAL_INDEX "aMaterialIndex"
#define ATTR_NAME_TEXTURE_INDEX "aTextureIndex"

static const uint ATTR_LOCATION_POS = 0;
static const uint ATTR_LOCATION_UV = ATTR_LOCATION_POS + 1;
static const uint ATTR_LOCATION_NORM = ATTR_LOCATION_UV + 1;
static const uint ATTR_LOCATION_TANGENT = ATTR_LOCATION_NORM + 1;
static const uint ATTR_LOCATION_BITANGENT = ATTR_LOCATION_TANGENT + 1;
static const uint ATTR_LOCATION_MODEL = ATTR_LOCATION_BITANGENT + 1;
static const uint ATTR_LOCATION_INV_MODEL = ATTR_LOCATION_MODEL + 4;
static const uint ATTR_LOCATION_MATERIAL_INDEX = ATTR_LOCATION_INV_MODEL + 4;
static const uint ATTR_LOCATION_TEXTURE_INDEX =
    ATTR_LOCATION_MATERIAL_INDEX + 1;

typedef struct {
  uint location;
  const char *name;
} attributeSpec;

static const attributeSpec attributes[9] = {
    {ATTR_LOCATION_POS, ATTR_NAME_POS},
    {ATTR_LOCATION_UV, ATTR_NAME_UV},
    {ATTR_LOCATION_NORM, ATTR_NAME_NORM},
    {ATTR_LOCATION_TANGENT, ATTR_NAME_TANGENT},
    {ATTR_LOCATION_BITANGENT, ATTR_NAME_BITANGENT},
    {ATTR_LOCATION_MODEL, ATTR_NAME_MODEL},
    {ATTR_LOCATION_INV_MODEL, ATTR_NAME_INVMODEL},
    {ATTR_LOCATION_MATERIAL_INDEX, ATTR_NAME_MATERIAL_INDEX},
    {ATTR_LOCATION_TEXTURE_INDEX, ATTR_NAME_TEXTURE_INDEX},
};

void initShaderProgram(ShaderProgram *shaderProg);
void initShaderProgramFromFiles(ShaderProgram *shaderProgram,
                                const char *vsPath, const char *fsPath);

void initShaderFromFile(Shader *shader, const char *path);

void useShaderProgram(ShaderProgram *shaderProg);
void unbindShaderProgram();

void attachShadersToProgram(ShaderProgram *program, Shader shaders[],
                            int length);

void freeShader(Shader *shader);
void freeShaderProgram(ShaderProgram *shader);
