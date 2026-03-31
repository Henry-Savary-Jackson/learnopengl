#version 460 core
#extension GL_ARB_bindless_texture : require
layout (bindless_sampler) uniform;

in vec3 aPos;
in mat4 aModel;
layout (location = 15 ) in uint lightIndex;

flat out uint currentLightIndex;

layout(std140) uniform ViewProjection{
    mat4 view;
    mat4 projection;
    mat4 projectionTimesView;
    vec3 cameraPos;
    vec3 cameraDirection;
    vec2 screenSize;
};

void main()
{
    currentLightIndex = lightIndex;
    gl_Position = projectionTimesView * aModel  * vec4(aPos, 1.0);
}
