#pragma once

#include <learnopengl/engine/mesh.h>

#define TEXTURE_UNDEFINED_INDEX -1

void initTexture(Texture* texture,const char* name,const char * path);
void freeTexture( Texture* texture );
