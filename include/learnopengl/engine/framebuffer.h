#pragma once
#include "learnopengl/engine/mesh.h"
#include "learnopengl/engine/shader.h"

typedef struct {
    uint glID;
    uint attachment;
} RenderBuffer;

typedef struct {
    uint glID;
    uint vao;
    uint quadVBO;
    uint uvVBO;
    uint width;
    uint height;
    Texture** textures;
    Sampler sampler;
    RenderBuffer renderBuffer;
    uint numTextures;
}FrameBuffer;

void initRenderBuffer(RenderBuffer* renderBuffer, uint attachment);
void freeRenderBuffer(RenderBuffer* renderBuffer);

void initFrameBuffer(FrameBuffer* framebuffer, uint numTextures, Texture** textures);
void renderFrameBuffer(FrameBuffer* framebuffer, uint width, uint height, ShaderProgram* shaderProgram, const char ** names);
void freeFrameBuffer(FrameBuffer *frameBuffer) ;
void bindFrameBuffer(FrameBuffer* framebuffer, uint width, uint height);
void unbindFrameBuffer();
