#version 460 core
#extension GL_ARB_bindless_texture : require
layout (bindless_sampler) uniform;


in vec2 texC;

uniform sampler2D gPos;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;
uniform sampler2D gTangent;
uniform sampler2D gBiTangent;
uniform sampler2D gNormalMap;
uniform sampler2D gSpecularMap;
uniform usampler2D gMaterialIndex;

out vec4 FragColor;
void main()
{
    FragColor =  vec4(0,1,1,1);
}
