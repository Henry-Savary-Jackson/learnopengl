#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 1 ) in vec2 aTexPos;
layout (location = 2 ) in uint modelIndex;
layout (location = 3 ) in vec3 aNormal;

struct LightSource
{
    vec4 position;
    vec4 colour;
    float intensity;
};

out vec2 texPos ;
out vec3 normalCamera;
out vec3 fragPosCamera;
out vec3 lightPosCamera;
flat out LightSource[] lightData;

layout(std140) uniform ViewProjection{
    mat4 view;
    mat4 projection;
};


layout (std430, binding=3) buffer ModelTransforms 
{
    mat4 models[];
} ;

layout (std140) uniform lights{
    LightSource[] lightData;
} lights;


uniform vec3 lightPos;

void main()
{
    texPos = aTexPos;
    lightPosCamera = vec3( view *  vec4(lightPos,1.0));

    normalCamera = vec3( transpose(inverse(models[modelIndex])) * vec4(aNormal,1));

    normalCamera = normalize( vec3( view * vec4(normalCamera,0)));

    vec4 vertexPosCamera = view * models[modelIndex]* vec4(aPos , 1.0);
    fragPosCamera = vec3(vertexPosCamera);
    gl_Position = projection * vertexPosCamera ;
}