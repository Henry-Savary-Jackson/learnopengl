#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 1 ) in vec2 texCoord;
layout (location = 2 ) in uint modelIndex;

out vec2 texPos ;
flat out vec3 colour;

layout(std140) uniform ViewProjection{
    mat4 view;
    mat4 projection;
    mat4 projectionTimesView;
};
struct LightSource
{
    vec4 positionCamera;
    vec4 colour;
    float power;
};


layout (std140) buffer ModelTransforms 
{
    mat4 models[];
} ;

layout (std430) buffer Lights{
    uint numLights;
    LightSource lightData[];
} ;

void main()
{
    texPos = texCoord;
    colour = lightData[modelIndex].colour.rgb;
    gl_Position = projectionTimesView  *models[modelIndex] * vec4(aPos  , 1.0) ;
}