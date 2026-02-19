#version 460 core
#extension GL_ARB_bindless_texture : require
layout (bindless_sampler) uniform;

in vec2 texPos;
in vec3 normalCamera;
in vec3 fragPosCamera;
in vec3 fragPos;
flat in uint materialIndex;
flat in uint  textureIndexFrag;

out vec4 FragColor;


struct Material {
    vec4 colour;
    vec3 coeffs;
    float reflectionIndex;
    float refractionIndex;
    uint specularMapIndex;
    uint normalMapIndex;
    uint textureMapIndex;
};

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

layout (std140) uniform Materials{
    Material materials[100];
};

layout (std140) uniform  Textures{
   uvec2 textures[100];
} ;

layout(std140) uniform ViewProjection{
    mat4 view;
    mat4 projection;
    mat4 projectionTimesView;
    vec3 cameraPos;
    vec3 cameraDirection;
};

// get the contribution of one point light to the pixel
vec3 getColourLight( inout LightSource light, inout Material material){

    vec3 lightColour = light.colour.rgb;
    float ambientCoeff = material.coeffs.x;
    // float ambientCoeff = 4;
    vec3 ambient =   lightColour  * ambientCoeff;
    vec3 materialColour = material.colour.rgb;
    // vec3 materialColour = vec3(1,1,1);

    float power = light.power;
    vec3 lightPosCamera = light.positionCamera.xyz;

    vec3 lightFragDiff = lightPosCamera-fragPosCamera;
    float intensity = 1/pow(length(lightFragDiff),2);

    float diffuseCoeff = material.coeffs.y;
    float specularCoeff = material.coeffs.z;

    vec3 lightDir = normalize( lightFragDiff );
    vec3 norm = normalize(normalCamera);
    vec3 diffuse = clamp(dot( norm , lightDir) ,0, 1)  *lightColour* diffuseCoeff;

    float spec =  clamp(dot(reflect(lightDir,norm), normalize(fragPosCamera)  ),0,1) ;
    vec3 specular = pow(spec,128) * lightColour * specularCoeff;;

    vec3 colour =  ambient + (diffuse +specular)*materialColour*intensity ;
    return colour;
}

void main()
{
    vec3 colour = vec3(0,0,0);
    Material material = materials[materialIndex];
    for (int i = 0 ; i < numLights.x; i ++){
        colour += getColourLight(lightData[i], material );
    }
    FragColor = texture(sampler2D(textures[textureIndexFrag]), texPos) *  vec4(colour,1) ;

}
