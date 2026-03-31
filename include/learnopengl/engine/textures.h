#pragma once

#include <learnopengl/engine/mesh.h>

#define TEXTURE_UNDEFINED_INDEX -1

void initTexture(Texture *texture, const char *name, uint textureType, uint format, uint internalFormat);

void loadTexture2D(Texture *texture, const char *path);

void loadTextureCubeMap(Texture *texture, const char *path,
                        const char *sides[6]);
void setTextureParameter(Texture *texture, uint key, uint value);

void bindTexture(Texture* texture);
void unbindTexture(Texture* texture);

void freeTexture(Texture *texture);

void initSampler(Sampler *sampler);
void setSamplerParameter(Sampler *sampler, uint key, uint value);

void freeSampler(Sampler *sampler);
