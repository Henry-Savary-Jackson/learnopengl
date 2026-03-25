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
    Texture texture;
    Sampler sampler;
    RenderBuffer renderBuffer;
    ShaderProgram * shaderProgram;
}FrameBuffer;

void initRenderBuffer(RenderBuffer* renderBuffer, uint attachment);
void freeRenderBuffer(RenderBuffer* renderBuffer);

void initFrameBuffer(FrameBuffer* framebuffer, uint width, uint height);
void renderFrameBuffer(FrameBuffer* framebuffer, uint width, uint height);
void attachShaderProgramToFrameBuffer(FrameBuffer* framebuffer, ShaderProgram* shaderProgram);
void freeFrameBuffer(FrameBuffer *frameBuffer) ;
void bindFrameBuffer(FrameBuffer* framebuffer, uint width, uint height);
void unbindFrameBuffer(FrameBuffer* frameBuffer);
