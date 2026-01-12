#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1 ) in vec2 texCoord;

out vec2 texC ;

uniform mat4 mvp;

void main()
{
    texC = texCoord;
    gl_Position = mvp * vec4(aPos, 1.0);
}