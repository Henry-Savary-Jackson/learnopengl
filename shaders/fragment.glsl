#version 430 core

in vec2 texPos;
in vec3 normalCamera;
in vec3 fragPosCamera;  
in vec3 lightPosCamera;

out vec4 FragColor;

uniform sampler2D texture0;
uniform sampler2D texture1;

layout ( std140) uniform Material{
    float ambientCoeff;
    float diffuseCoeff;
    float specularCoeff;
    vec4 colour;
} material;


uniform vec3 lightColour;

layout (std140) uniform Light{
    vec4 position;
    vec4 colour;
    float intensity;
} light;

void main()
{
    float lightIntensity = 10;
    float distFactor = 1/pow(length(lightPosCamera-fragPosCamera),2);
    vec3 lightDir = normalize( lightPosCamera-fragPosCamera );
    vec3 norm = normalize(normalCamera);
    vec3 diffuse = clamp(dot( norm , lightDir) ,0, 1)  *lightColour;
    vec3 ambient =   lightColour ;

    float spec =  clamp(dot(reflect(lightDir,norm), normalize(fragPosCamera)  ),0,1) ;
    vec3 specular = pow(spec,64) *lightColour;

    
    vec3 colour =  (ambient+ specular)*material.colour.xyz*distFactor*lightIntensity  ;   
    FragColor = mix(texture(texture0, texPos), texture(texture1, texPos) ,0.5) * vec4(colour, 1) ;
} 