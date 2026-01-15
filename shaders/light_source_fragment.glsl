
#version 430 core

out vec4 FragColor;

uniform vec3 lightColour;

void main()
{
    FragColor =  vec4(lightColour, 1) ;
}