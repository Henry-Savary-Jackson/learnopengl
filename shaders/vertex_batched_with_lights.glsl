#version 460 core
#extension GL_ARB_bindless_texture : require
// #extension GL_EXT_nonuniform_qualifier : require

layout (location = 0) in vec3 aPos;
layout (location = 1 ) in vec2 aTexPos;
layout (location = 2 ) in vec3 aNormal;
layout (location = 3) in mat4 model;
layout (location = 7) in mat4 invModel;
layout (location = 11) in uint matIndex;
layout (location = 12) in uint textureIndex;

out vec2 texPos ;
out vec3 normalCamera;
out vec3 fragPosCamera;
out vec3 fragPos;
flat out uint materialIndex;
flat out uint textureIndexFrag;

layout(std140) uniform ViewProjection{
    mat4 view;
    mat4 projection;
    mat4 projectionTimesView;
    vec3 cameraPos;
    vec3 cameraDirection;
};

void main()
{
    texPos = aTexPos;

    // invModel is transpose(inverse(model))
    // precomputed for efficiency
    normalCamera = normalize(  vec3( invModel* vec4(aNormal,1))   );

    // normalCamera = normalize( vec3( view * vec4(normalCamera,0)));

    vec4 modelPos = model * vec4(aPos , 1.0);
    fragPos = vec3(modelPos);
    vec4 vertexPosCamera = view * modelPos;

    fragPosCamera = vec3(vertexPosCamera);

    gl_Position = projection * vertexPosCamera ;

    materialIndex = matIndex;

    textureIndexFrag = textureIndex;
}
