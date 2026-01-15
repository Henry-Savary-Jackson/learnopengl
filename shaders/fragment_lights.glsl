#version 430 core

in vec2 texPos;
in vec3 normalCamera;
in vec3 fragPosCamera;  

out vec4 FragColor;

uniform sampler2D texture0;
uniform sampler2D texture1;

struct LightSource
{
    vec4 positionCamera;
    vec4 colour;
    float power;
};

uniform uint numLights;

layout (std430, binding=0) buffer Lights{
    LightSource lightData[];
} ;

// avoid using vec3 to avoid std140 issues
layout ( std140) uniform Material{
    float ambientCoeff;
    float diffuseCoeff;
    float specularCoeff;
    float padd;
    vec4 colour;
    // in this order so that i can align structs the same way CPU side
} material;


// get the contribution of one point light to the pixel
vec3 getColourLight( LightSource light){

    vec3 lightColour = light.colour.rgb;
    // vec3 lightColour = vec3(1,0,0);
    vec3 lightPosCamera = light.positionCamera.xyz;
    // vec3 lightPosCamera = vec3(1,0,0);
    float power = light.power;
    // float power = 10; 

    float ambientCoeff = material.ambientCoeff;
    float diffuseCoeff = material.diffuseCoeff;
    float specularCoeff = material.specularCoeff;
    // vec3 materialColour = material.colour.rgb;
    // float ambientCoeff =  1;
    // float diffuseCoeff = 1 ;
    // float specularCoeff = 0;
    vec3 materialColour = material.colour.rgb;


    vec3 lightFragDiff = lightPosCamera-fragPosCamera;
    float distFactor = 1/pow(length(lightFragDiff),2);
    vec3 ambient =   lightColour  * ambientCoeff;

    vec3 lightDir = normalize( lightFragDiff );
    vec3 norm = normalize(normalCamera);
    vec3 diffuse = clamp(dot( norm , lightDir) ,0, 1)  *lightColour* diffuseCoeff;

    float spec =  clamp(dot(reflect(lightDir,norm), normalize(fragPosCamera)  ),0,1) ;
    vec3 specular = pow(spec,64) * lightColour * specularCoeff;
    
    vec3 colour =  (ambient+diffuse +specular)*distFactor*power ;   
    return colour;
}

void main()
{
    vec3 colour = vec3(0,0,0);
    for (int i = 0 ; i < numLights; i ++){
        colour += getColourLight(lightData[i]);
    }
    FragColor = mix(texture(texture0, texPos), texture(texture1, texPos) ,0.5) * vec4(colour, 1) ;
} 