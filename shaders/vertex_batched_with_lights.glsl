#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1 ) in vec2 aTexPos;
layout (location = 2 ) in vec3 aNormal;
layout (location = 3) in mat4 model;
layout (location = 7) in mat4 invModel;
layout (location = 11) in vec3 materialColour;
layout (location = 12) in vec3 coefficients;
layout (location = 13) in sampler2D textureIndex;

out vec2 texPos ;
out vec3 normalCamera;
out vec3 fragPosCamera;
out Material {
    vec3 colour;
    float ambientCoeff;
    float diffuseCoeff;
    float specularCoeff;
} material;
flat out sampler2D textureFrag;

layout(std140) uniform ViewProjection{
    mat4 view;
    mat4 projection;
    mat4 projectionTimesView;
};

void main()
{
    texPos = aTexPos;

    // invModel is transpose(inverse(model))
    // precomputed for efficiency
    normalCamera = vec3( invModel* vec4(aNormal,1));

    normalCamera = normalize( vec3( view * vec4(normalCamera,0)));

    vec4 vertexPosCamera = view * model * vec4(aPos , 1.0);

    fragPosCamera = vec3(vertexPosCamera);

    gl_Position = projection * vertexPosCamera ;

    material.colour = materialColour;
    material.ambientCoeff = coefficients.x;
    material.diffuseCoeff = coefficients.y;
    material.specularCoeff = coefficients.z;

    textureFrag = textureIndex;
}
