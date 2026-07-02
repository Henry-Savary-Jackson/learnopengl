#version 460 core
#extension GL_ARB_bindless_texture : require
layout (bindless_sampler) uniform;

flat in uint currentLightIndex;

struct LightSource
{
    vec4 position;
    vec4 colour;
    float power;
};

layout (std430) buffer Lights{
    uint numLights;
    LightSource lightData[];
} ;

out vec4 col;

void main(){

    // col = lightData[currentLightIndex].colour;
    // for the first geometry pass we dont write to color buffer and only to stencil buffer
}
