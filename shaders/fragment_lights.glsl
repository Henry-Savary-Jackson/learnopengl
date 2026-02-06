#version 460 core
#extension GL_ARB_bindless_texture : require

in vec2 texPos;
in vec3 normalCamera;
in vec3 fragPosCamera;
in Material {
    vec3 colour;
    float ambientCoeff;
    float diffuseCoeff;
    float specularCoeff;
} material;
flat in uint  textureIndexFrag;

out vec4 FragColor;


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

layout (std430) readonly buffer Textures{
   sampler2D textures[50];
} ;

// get the contribution of one point light to the pixel
vec3 getColourLight( inout LightSource light){

    vec3 lightColour = light.colour.rgb;
    float ambientCoeff = material.ambientCoeff;
    vec3 ambient =   lightColour  * ambientCoeff;
    vec3 materialColour = material.colour;

    float power = light.power;
    vec3 lightPosCamera = light.positionCamera.xyz;

    vec3 lightFragDiff = lightPosCamera-fragPosCamera;
    float intensity = 1/pow(length(lightFragDiff),2);

    float diffuseCoeff = material.diffuseCoeff;
    float specularCoeff = material.specularCoeff;

    vec3 lightDir = normalize( lightFragDiff );
    vec3 norm = normalize(normalCamera);
    vec3 diffuse = clamp(dot( norm , lightDir) ,0, 1)  *lightColour* diffuseCoeff;

    float spec =  clamp(dot(reflect(lightDir,norm), normalize(fragPosCamera)  ),0,1) ;
    vec3 specular = pow(spec,128) * lightColour * specularCoeff;;

    vec3 colour =  (ambient+diffuse +specular)*materialColour*intensity ;
    return colour;
}

void main()
{
    vec3 colour = vec3(0,0,0);
    for (int i = 0 ; i < numLights.x; i ++){

        colour += getColourLight(lightData[i]);
    }
    // FragColor =  vec4(colour,1) ;
    // sampler2D s = textures[textureIndexFrag];
    // if (textures[textureIndexFrag+1] == 1){
        // FragColor =  vec4(1,0,0 ,1) ;
    // }else{
        // FragColor =  vec4(0,1,0 ,1) ;
    FragColor = texture(textures[textureIndexFrag], texPos) *  vec4(colour,1) ;
    // }

}
