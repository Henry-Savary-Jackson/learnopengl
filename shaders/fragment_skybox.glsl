#version 460 core
#extension GL_ARB_bindless_texture : require
layout (bindless_sampler) uniform;

in vec3 texPos;
uniform uint textureIndex;

out vec4 FragColor;

layout (std430) uniform Textures{
   uvec2 textures[100];
} ;

void main()
{
    FragColor = texture(samplerCube(textures[textureIndex]), texPos) ;
}
