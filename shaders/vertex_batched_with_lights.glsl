#version 460 core
#extension GL_ARB_bindless_texture : require
layout (bindless_sampler) uniform;

in vec3 aPos;
in vec2 aTexPos;
in vec3 aNormal;
in vec3 aTangent;
in vec3 aBiTangent;
in mat4 aModel;
in mat4 aInvModel;
in uint aMaterialIndex;
in uint aTextureIndex;

out VS_OUT{
 vec2 texPos ;
 vec3 normal;
 vec3 fragPos;
 vec3 tangent;
 vec3 bitangent;
};
flat out uint materialIndex;
flat out uint textureIndexFrag;

layout(std140) uniform ViewProjection{
    mat4 view;
    mat4 projection;
    mat4 projectionTimesView;
    vec3 cameraPos;
    vec3 cameraDirection;
    vec2 screenSize;
};

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


void main()
{
    texPos = aTexPos;

    // invModel is transpose(inverse(model))
    // precomputed for efficiency


    tangent = normalize(vec3(aInvModel * vec4(aTangent,   0.0)));
    bitangent = normalize(vec3(aInvModel * vec4(aBiTangent, 0.0)));
    normal = normalize(  vec3( aInvModel* vec4(aNormal,0))   );

    vec4 posWorldSpace = aModel * vec4(aPos , 1.0);
    fragPos = vec3(posWorldSpace);

    vec4 vertexPosCamera = view *  posWorldSpace;

    gl_Position = projectionTimesView * posWorldSpace ;

    materialIndex = aMaterialIndex;

    textureIndexFrag = aTextureIndex;
}
