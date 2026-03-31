#version 460 core
#extension GL_ARB_bindless_texture : require
layout (bindless_sampler) uniform;

flat in uint currentLightIndex;

void main(){
    // for the first geometry pass we dont write to color buffer and only to stencil buffer
}
