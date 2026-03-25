#version 460 core

layout in vec3 aPos;
layout in vec2 aUV;

out vec2 texC;

void main()
{
    texC = aUV;
    gl_Position = vec4(aPos.x, aPos.y, 0, 1.0);
}
