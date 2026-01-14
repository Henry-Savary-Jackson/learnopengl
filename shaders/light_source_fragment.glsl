
#version 430 core

out vec4 FragColor;

layout (std140) uniform Light{
    vec4 position;
    vec4 colour;
    float intensity;
} light;

uniform vec3 lightColour;

void main()
{
    FragColor =  vec4(1,1,1, 1) ;
}