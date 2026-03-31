#version 460 core
#extension GL_ARB_bindless_texture : require
layout (bindless_sampler) uniform;

in VS_OUT{
 vec2 texPos;
 vec3 normal;
 vec3 fragPos;
 vec3 tangent;
 vec3 bitangent;
};
flat in uint materialIndex;
flat in uint textureIndexFrag;

layout (location = 0) out vec3 gPos;
layout (location = 1) out vec3 gAlbedo;
layout (location = 2) out vec3 gNormal;
layout (location = 3) out vec3 gTangent;
layout (location = 4) out vec3 gBiTangent;
layout (location = 5) out uint gMaterialIndex;
layout (location = 6) out vec3 gNormalMap;
layout (location = 7) out vec3 gSpecularMap;



struct Material {
    vec4 colour;
    vec4 coeffs;
    float refractionIndex;
    float reflectionStrength;
    int specularMapIndex;
    int normalMapIndex;
};


layout (std140) uniform Materials{
    Material materials[100];
};

layout (std140) uniform  Textures{
   uvec2 textures[100];
} ;

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
    Material material = materials[materialIndex];
    gPos = fragPos;
    gNormal = normal;
    vec3 texCol = vec3(0);
    if (textureIndexFrag != -1)
        vec3 texCol = texture(sampler2D(textures[textureIndexFrag]), texPos).rgb ;
    gAlbedo = normalize(texCol + material.colour.rgb);
    gTangent = tangent;
    gBiTangent = bitangent;
    gMaterialIndex = materialIndex;
    gNormalMap = material.normalMapIndex == -1? vec3(0): texture(sampler2D(textures[material.normalMapIndex]), texPos).rgb;
    gSpecularMap = material.specularMapIndex == -1? vec3(0): texture(sampler2D(textures[material.specularMapIndex]), texPos).rgb;
}
