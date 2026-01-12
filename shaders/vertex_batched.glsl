#version 430 core
layout (location = 0) in vec3 aPos;
layout (location = 1 ) in vec2 texCoord;
layout (location = 2 ) in uint modelIndex;

out vec2 texC ;

uniform mat4 vp;

layout (std430, binding = 2) buffer MatArr
{
    mat4 mats[];
} ;

void main()
{
    texC = texCoord;

    gl_Position = vp   * mats[modelIndex]*  vec4(aPos  , 1.0) ;
}