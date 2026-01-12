#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1 ) in vec2 texCoord;
layout (location = 2 ) in uint modelIndex;

out vec2 texC ;

uniform (location = 0)mat4 projection;
uniform (location = 1)mat4 view;

layout (std430, binding = 0)
buffer MatArr
{
    mat4 mats[];
} transforms;

void main()
{
    texC = texCoord;
    gl_Position = projection* view *transforms.mats[modelIndex] * vec4(aPos, 1.0);
}