#version 460 core
#extension GL_ARB_bindless_texture : require
layout (bindless_sampler) uniform;



uniform uint gPos;
uniform uint gNormal;
uniform uint gAlbedo;
uniform uint gTangent;
uniform uint gBiTangent;
uniform uint gNormalMap;
uniform uint gSpecularMap;
uniform uint gMaterialIndex;

layout (location= 0) out vec4 FragColor;

flat in uint currentLightIndex;


struct Material {
    vec4 colour;
    vec4 coeffs;
    float refractionIndex;
    float reflectionStrength;
    int specularMapIndex;
    int normalMapIndex;
};

uniform uint skyboxCubeMapIndex;

struct LightSource
{
    vec4 position;
    vec4 colour;
    float power;
};

layout (std430) buffer Lights{
    uint numLights;
    LightSource lightData[];
} ;

layout (std140) uniform  Textures{
   uvec2 textures[100];
} ;

layout(std140) uniform ViewProjection{
    mat4 view;
    mat4 projection;
    mat4 projectionTimesView;
    vec3 cameraPos;
    vec3 cameraDirection;
    vec2 screenSize;
};

layout (std140) uniform Materials{
    Material materials[100];
};

vec3 getDiffuseFromNormalMap( inout LightSource light, inout Material material, vec2 texPos){

    vec3 tangent = texture(sampler2D(textures[gTangent]), texPos ).rgb;
    vec3 bitangent = texture(sampler2D(textures[gBiTangent]), texPos ).rgb;
    vec3 normal = texture(sampler2D(textures[gNormal]), texPos ).rgb;
    vec3 fragPos = texture(sampler2D(textures[gPos]), texPos ).rgb;

    mat3 TBN = transpose(mat3(tangent,bitangent,normal));

    vec3 lightPosTangent = TBN * light.position.xyz;
    vec3 fragPosTangent = TBN * fragPos;

    vec3 lightFragDiff = lightPosTangent-fragPosTangent;

    float diffuseCoeff = material.coeffs.y;

    vec3 lightDir = normalize( lightFragDiff );
    vec3 diffuse = clamp(dot( normal , lightDir) ,0, 1)  *light.colour.xyz* diffuseCoeff;
    return diffuse;
}

// get the contribution of one point light to the pixel
vec3 getColourLight(  inout LightSource light, inout Material material, vec2 texPos){
    vec3 lightColour = light.colour.rgb;
    float ambientCoeff = material.coeffs.x;

    vec3 fragPos = texture(sampler2D(textures[gPos]), texPos ).rgb;
    vec3 normal = texture(sampler2D(textures[gNormal]), texPos ).rgb;

    vec3 ambient =   lightColour  * ambientCoeff;
    vec3 materialColour = material.colour.rgb;

    float power = light.power;
    vec3 lightPos = light.position.xyz;

    vec3 lightFragDiff = lightPos-fragPos;
    float intensity = power/pow(length(lightFragDiff),2);

    float specularCoeff = material.coeffs.z;

    vec3 lightDir = normalize( lightFragDiff );
    vec3 diffuse = vec3(0,0,0);

    diffuse = getDiffuseFromNormalMap(light, material, texPos);

    float spec =  clamp(dot(reflect(lightDir,normal), normalize(fragPos)  ),0,1) ;
    vec3 specular = pow(spec,128) * lightColour * specularCoeff;

    return ambient + (diffuse +specular)*materialColour*intensity ;;
}


vec2 calcTexCoord( ){
    return gl_FragCoord.xy  / screenSize;
}

void main()
{

    vec2 texCoord = calcTexCoord();

    uint materialIndex = texture(usampler2D(textures[gMaterialIndex]),texCoord).r;
    Material material = materials[materialIndex];
    LightSource light = lightData[currentLightIndex];
    vec3 normal = texture(sampler2D(textures[gNormal]), texCoord).rgb;
    vec3 fragPos = texture(sampler2D(textures[gPos]), texCoord).rgb;
    vec3 I = normalize(fragPos - cameraPos);
    vec3 R = refract(I, normal, material.refractionIndex);
    vec3 skyboxPixel  = texture(samplerCube(textures[skyboxCubeMapIndex]), R).rgb;
    vec3 colour = getColourLight(light, material, texCoord);
    FragColor =  vec4(skyboxPixel*colour, 1);
}
