#version 460 core
#extension GL_ARB_bindless_texture : require
layout (bindless_sampler) uniform;

in VS_OUT{
 vec2 texPos ;
 vec3 normal;
 vec3 fragPos;
 vec3 tangent;
 vec3 bitangent;
};

flat in uint materialIndex;
flat in uint textureIndexFrag;

layout (location=0) out vec4 FragColor;
// layout (location=1) out vec4 BrightColor;


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
    vec2 screenSize;
};

vec3 getDiffuseFromNormalMap( int lightIndex, inout Material material){
    LightSource light = lightData[lightIndex];

    mat3 TBN = transpose(mat3(tangent,bitangent,normal));

    vec3 lightPosTangent = TBN * light.position.xyz;
    vec3 fragPosTangent = TBN * fragPos;

    vec3 lightFragDiff = lightPosTangent-fragPosTangent;

    float diffuseCoeff = material.coeffs.y;

    vec3 lightDir = normalize( lightFragDiff );
    vec3 norm = texture( sampler2D(textures[material.normalMapIndex]), texPos).rgb;
    vec3 diffuse = clamp(dot( norm , lightDir) ,0, 1)  *light.colour.xyz* diffuseCoeff;
    return diffuse;
}

// get the contribution of one point light to the pixel
vec3 getColourLight(  int lightIndex, inout Material material){
    LightSource light = lightData[lightIndex];

    vec3 lightColour = light.colour.rgb;
    float ambientCoeff = material.coeffs.x;
    // float ambientCoeff = 4;
    vec3 ambient =   lightColour  * ambientCoeff;
    vec3 materialColour = material.colour.rgb;
    // vec3 materialColour = vec3(1,1,1);

    float power = light.power;
    vec3 lightPos = light.position.xyz;

    vec3 lightFragDiff = lightPos-fragPos;
    float intensity = 1/pow(length(lightFragDiff),2);

    float specularCoeff = material.coeffs.z;

    vec3 lightDir = normalize( lightFragDiff );
    vec3 norm = normalize(normal);
    vec3 diffuse = vec3(0,0,0);

    if (material.normalMapIndex != -1){
        diffuse = getDiffuseFromNormalMap(lightIndex, material);
    }else{
        float diffuseCoeff = material.coeffs.y;
        diffuse = clamp(dot( norm , lightDir) ,0, 1)  *lightColour* diffuseCoeff;
    }
    // float diffuseCoeff = material.coeffs.y;
    // diffuse = clamp(dot( norm , lightDir) ,0, 1)  *lightColour* diffuseCoeff;

    float spec =  clamp(dot(reflect(lightDir,norm), normalize(fragPos)  ),0,1) ;
    vec3 specular = pow(spec,128) * lightColour * specularCoeff;;

    vec3 colour =  ambient + (diffuse +specular)*materialColour*intensity ;
    return colour;
}

void main()
{
    vec3 colour = vec3(0,0,0);
    Material material = materials[materialIndex];
    for (int i = 0 ; i < numLights.x; i ++){
        colour += getColourLight(i, material );
    }
    vec3 I = normalize(fragPos - cameraPos);
    vec3 R = refract(I, normalize(normal), material.refractionIndex);
    // FragColor = texture(sampler2D(textures[textureIndexFrag]), texPos) ;
    FragColor = vec4(texture(samplerCube(textures[skyboxCubeMapIndex]), R).rgb* material.reflectionStrength, 1) ;
    FragColor *= vec4(colour,1) ;

    // float brightness = dot(FragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
    // if (brightness >= 1){
    //     BrightColor = FragColor;

    // }else{
    //     BrightColor = vec4(0,0,0,0);
    // }



}
