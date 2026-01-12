#pragma once

#include "mesh.h"


void bindTexturesUniform(ShaderProgram *shaderProgram, Texture *textures,
                         int numTextures) ;
void initTexture(Texture* texture,const char* name,const char * path);
void freeTexture( Texture* texture );
