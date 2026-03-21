#version 430 core
layout in vec3 aPos;
layout in vec2 aUV;

out vec2 texC ;

uniform mat4 mvp;

void main()
{
    texC = texCoord;
    gl_Position = mvp * vec4(aPos, 1.0);
}
