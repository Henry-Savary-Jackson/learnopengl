#version 330 core
in vec2 texC;
out vec4 FragColor;

uniform sampler2D texture0;
uniform sampler2D texture1;

void main()
{
    FragColor = mix(texture(texture0, texC), texture(texture1, texC) ,0.4);
} 