
#version 430 core

flat in vec3 colour;

layout (location = 0)out vec4 FragColor;
layout (location = 1)out vec4 BrightColor;


void main()
{
    FragColor =  vec4(colour, 1) ;
    BrightColor = FragColor;
}
