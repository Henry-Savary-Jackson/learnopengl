#version 460 core
in vec3 aPos;
in vec2 aUV;
in uint aModelIndex;
in mat4 aModel;
in mat4 aInvModel;
out vec2 texPos ;
flat out vec3 colour;

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
    texPos = aUV;
    colour = lightData[gl_InstanceID].colour.rgb;
    gl_Position = projectionTimesView  * aModel * vec4(aPos  , 1.0) ;
}
