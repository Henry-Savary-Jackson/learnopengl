#version 460 core
#extension GL_ARB_bindless_texture : require
layout (bindless_sampler) uniform;



uniform sampler2D gPos;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;
uniform sampler2D gTangent;
uniform sampler2D gBiTangent;
uniform sampler2D gNormalMap;
uniform sampler2D gSpecularMap;
uniform usampler2D gMaterialIndex;

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

    vec3 tangent = texture(gTangent, texPos ).rgb;
    vec3 bitangent = texture(gBiTangent, texPos ).rgb;
    vec3 normal = texture(gNormal, texPos ).rgb;
    vec3 fragPos = texture(gPos, texPos ).rgb;

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

    const float constTerm = 1;
    const float linearTerm= 0.7;
    const float quadTerm  = 1.8;

    vec3 lightColour = light.colour.rgb;
    float ambientCoeff = material.coeffs.x;

    vec3 fragPos = texture(gPos, texPos ).rgb;
    vec3 normal = texture(gNormal, texPos ).rgb;

    vec3 ambient =   lightColour  * ambientCoeff;
    vec3 materialColour = material.colour.rgb;

    float power = light.power;
    vec3 lightPos = light.position.xyz;

    vec3 lightFragDiff = lightPos-fragPos;
    float r = length(lightFragDiff);

    float intensity = power/(pow(r,2)* quadTerm + r * linearTerm + constTerm);

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

    uint materialIndex = texture(gMaterialIndex,texCoord).r;
    Material material = materials[materialIndex];
     vec3 bitangent = texture(gBiTangent, texCoord ).rgb;
    LightSource light = lightData[currentLightIndex];
    vec3 normal = texture(gNormal, texCoord).rgb;
    vec3 fragPos = texture(gPos, texCoord).rgb;
    vec3 I = normalize(fragPos - cameraPos);
    vec3 R = refract(I, normal, material.refractionIndex);
    vec3 skyboxPixel  = texture(samplerCube(textures[skyboxCubeMapIndex]), R).rgb;
    vec3 colour = getColourLight(light, material, texCoord);
    FragColor =  vec4(colour*skyboxPixel, 1);
}
