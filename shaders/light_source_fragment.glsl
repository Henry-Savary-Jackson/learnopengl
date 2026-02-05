
#version 430 core

flat in vec3 colour;

out vec4 FragColor;


void main()
{
    FragColor =  vec4(colour, 1) ;
}
