#version 330 core
in vec3 color;
in vec2 texC;
out vec4 FragColor;

uniform sampler2D ourTexture;
uniform float t;

void main()
{
    FragColor = texture(ourTexture, texC) * vec4(color *(abs( sin(t)) + 0.6), 1.0) ;
} 