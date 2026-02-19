#include "learnopengl/engine/shader.h"
#include <assert.h>
#include <cglm/types.h>
#include <learnopengl/engine/skybox.h>
#include <glad/glad.h>
#include <stdlib.h>

const static char * SKYBOX_TEXTURE_UNIFORM_NAME= "skyboxCubeMapIndex";

const static float skyboxVertices[] = {
    // positions
    -1.0f,  1.0f, -1.0f,
    -1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f, -1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,

    -1.0f, -1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f, -1.0f,  1.0f,
    -1.0f, -1.0f,  1.0f,

    -1.0f,  1.0f, -1.0f,
     1.0f,  1.0f, -1.0f,
     1.0f,  1.0f,  1.0f,
     1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f,  1.0f,
    -1.0f,  1.0f, -1.0f,

    -1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f, -1.0f,
     1.0f, -1.0f, -1.0f,
    -1.0f, -1.0f,  1.0f,
     1.0f, -1.0f,  1.0f
};

void initSkybox(SkyboxManager* skybox){
    skybox->shaderProgram = NULL;
    skybox->textureIndex = 0;
    glGenVertexArrays(1, &skybox->vao);
    glGenBuffers(1, &skybox->vertexVbo);

    glBindVertexArray(skybox->vao);

    glBindBuffer(GL_ARRAY_BUFFER, skybox->vertexVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices) ,skyboxVertices, GL_DYNAMIC_DRAW );
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), NULL );
    glEnableVertexAttribArray(0);
}

void setSkyboxShader(SkyboxManager* skybox, ShaderProgram* shaderProgram){
    skybox->shaderProgram = shaderProgram;
}

void bindSkyboxManagerToShader(SkyboxManager* skybox, ShaderProgram* shaderProgram){
    uint location  = glGetUniformLocation(skybox->shaderProgram->id,  SKYBOX_TEXTURE_UNIFORM_NAME);
    if (location!= -1){
        glProgramUniform1ui(skybox->shaderProgram->id, location, skybox->textureIndex );
    }
}

void setSkyboxTextureIndex(SkyboxManager* skybox, uint textureIndex){
    skybox->textureIndex = textureIndex;
    bindSkyboxManagerToShader(skybox, skybox->shaderProgram);
}

void renderSkybox(SkyboxManager* skybox){
    assert(skybox->shaderProgram);
    useShaderProgram(skybox->shaderProgram);
    glDepthMask(GL_FALSE);

    glBindVertexArray(skybox->vao);
    glDrawArrays(GL_TRIANGLES, 0, (int)(sizeof(skyboxVertices)/(3*sizeof(float)))) ;

    glDepthMask(GL_TRUE);
    unbindShaderProgram();
}
