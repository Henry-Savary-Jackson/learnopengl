#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1 ) in vec2 aTexPos;
layout (location = 2 ) in vec3 aNormal;

out vec2 texPos ;
out vec3 normalCamera;
out vec3 fragPosCamera;
out vec3 lightPosCamera;

layout(std140) uniform ViewProjection{
    mat4 view;
    mat4 projection;
    mat4 projectionTimesView;
};

layout (std430) buffer ModelTransforms
{
    mat4 models[];
};

layout (std430) buffer InverseModelTransforms
{
    mat4 invModels[];
};

void main()
{
    texPos = aTexPos;

    normalCamera = vec3( transpose(invModels[gl_InstanceID]) * vec4(aNormal,1));

    normalCamera = normalize( vec3( view * vec4(normalCamera,0)));

    vec4 vertexPosCamera = view *models[gl_InstanceID]* vec4(aPos , 1.0);

    fragPosCamera = vec3(vertexPosCamera);

    gl_Position = projection *  vertexPosCamera ;
    // gl_Position =  vec4(0.5,0.5, 0.4,1) ;
    // gl_PointSize = 4;
    // gl_Position = vec4(aPos.xy, -0.4, 1) ;
}
