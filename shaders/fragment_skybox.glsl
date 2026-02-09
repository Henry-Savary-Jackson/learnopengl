#version 460 core
#extension GL_ARB_bindless_texture : require
layout (bindless_sampler) uniform;

in vec3 texPos;
flat in uint textureIndexFrag;

out vec4 FragColor;

layout (std430) readonly buffer Textures{
   uvec2 textures[100];
} ;

void main()
{
    FragColor = texture(samplerCube(textures[textureIndexFrag]), texPos) ;
}
