#version 460 core
#extension GL_ARB_bindless_texture : require

layout (location = 0) in vec3 aPos;

out vec3 texPos ;

layout(std140) uniform ViewProjection{
    mat4 view;
    mat4 projection;
    mat4 projectionTimesView;
    vec3 cameraPos;
    vec3 cameraDirection;
};


void main(){
    texPos = aPos;
    gl_Position =  ((projection * mat4(mat3(view)) * vec4(aPos,1))).xyww;
}
