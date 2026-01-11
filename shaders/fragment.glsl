#version 330 core
in vec3 color;
in vec2 texC;
out vec4 FragColor;

uniform sampler2D texture0;
uniform sampler2D texture1;
uniform float t;

void main()
{
    FragColor = mix(texture(texture0, texC), texture(texture1, texC) ,0.3*sin(t)+0.4) * vec4(color,1.0f) ;
} 