#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1 ) in vec3 aCol;
layout (location = 2 ) in vec2 texCoord;

out vec3 color ;
out vec2 texC ;

void main()
{
    color = aCol;
    texC = texCoord;
    gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
}