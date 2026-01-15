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

layout (std430) buffer Lights{
    uint numLights;
    LightSource lightData[];
} ;

// avoid using vec3 to avoid std140 issues
layout ( std140) buffer Material{
    float ambientCoeff;
    float diffuseCoeff;
    float specularCoeff;
    vec4 colour;
    // in this order so that i can align structs the same way CPU side
} material;


float getLightIntensity( inout LightSource light){
    vec3 lightPosCamera = light.positionCamera.xyz;
    vec3 lightFragDiff = lightPosCamera-fragPosCamera;
    return 1/pow(length(lightFragDiff),2);
}

// get the contribution of one point light to the pixel
vec3 getColourLight( inout LightSource light){

    vec3 lightColour = light.colour.rgb;
    float ambientCoeff = material.ambientCoeff;
    vec3 ambient =   lightColour  * ambientCoeff;
    vec3 materialColour = material.colour.rgb;

    float power = light.power;
    float intensity = getLightIntensity( light);
    // if (power *intensity < 0.1){
    //     return ambient * power*intensity * materialColour;
    // }

    vec3 lightPosCamera = light.positionCamera.xyz;

    float diffuseCoeff = material.diffuseCoeff;
    float specularCoeff = material.specularCoeff;

    vec3 lightFragDiff = lightPosCamera-fragPosCamera;
    vec3 lightDir = normalize( lightFragDiff );
    vec3 norm = normalize(normalCamera);
    vec3 diffuse = clamp(dot( norm , lightDir) ,0, 1)  *lightColour* diffuseCoeff;

    float spec =  clamp(dot(reflect(lightDir,norm), normalize(fragPosCamera)  ),0,1) ;
    vec3 specular = pow(spec,128) * lightColour * specularCoeff;
    
    vec3 colour =  (ambient+diffuse +specular)*materialColour*intensity ;   
    return colour;
}

void main()
{
    vec3 colour = vec3(0,0,0);
    // vec3 sumPos  = vec3(0,0,0);
    // vec3 sumColor  = vec3(0,0,0);
    // uint numSkipped = 0;
    // float sumPower = 0;
    // float sumIntensity = 0;
    for (int i = 0 ; i < numLights.x; i ++){
        // if (intensity < 0.4){
        //     sumPos += lightData[i].positionCamera.xyz;
        //     sumColor = lightData[i].colour.xyz; 
        //     sumPower += intensity;
        //     numSkipped += 1;
        //     continue;
        // }

        colour += getColourLight(lightData[i]);
    }
    // add the average effect of faraway lights
    // LightSource avgFar = LightSource(vec4(sumPos,0)/numSkipped, vec4(sumColor,0)/numSkipped, sumPower);
    // colour += getColourLight(avgFar, sumIntensity);

    FragColor = mix(texture(texture0, texPos), texture(texture1, texPos) ,0.5) * vec4(colour,1) ;
} 