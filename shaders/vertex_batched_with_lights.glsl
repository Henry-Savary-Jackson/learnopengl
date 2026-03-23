#version 460 core
#extension GL_ARB_bindless_texture : require
// #extension GL_EXT_nonuniform_qualifier : require

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
 vec3 normalCamera;
 vec3 fragPosCamera;
 vec3 fragPos;
 vec3 tangent;
 vec3 bitangent;
 vec3 N;
};
flat out uint materialIndex;
flat out uint textureIndexFrag;

layout(std140) uniform ViewProjection{
    mat4 view;
    mat4 projection;
    mat4 projectionTimesView;
    vec3 cameraPos;
    vec3 cameraDirection;
};

struct LightSource
{
    vec4 positionCamera;
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
    normal = normalize(  vec3( aInvModel* vec4(aNormal,1))   );

    normalCamera = normalize( vec3( view * vec4(normal,0)));

    tangent = normalize(vec3(aInvModel * vec4(aTangent,   0.0)));
    bitangent = normalize(vec3(aInvModel * vec4(aBiTangent, 0.0)));
    N = normalize(vec3(aInvModel * vec4(aNormal,    0.0)));

    vec4 modelPos = aModel * vec4(aPos , 1.0);
    fragPos = vec3(modelPos);

    vec4 vertexPosCamera = view * modelPos;

    fragPosCamera = vec3(vertexPosCamera);

    gl_Position = projection * vertexPosCamera ;

    materialIndex = aMaterialIndex;

    textureIndexFrag = aTextureIndex;
}
