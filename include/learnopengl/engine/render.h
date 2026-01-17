
#include <learnopengl/engine/camera.h>
#include <learnopengl/engine/mesh.h>
#include <learnopengl/engine/object.h>
#include <sys/types.h>

typedef struct{
    uint vao;
    uint vbo;
    uint ebo;
    Vertex* vertexBuffer;
    uint* elementBuffer;
    Camera* camera;
} Renderer ;

void initRenderer(Renderer* renderer, Camera* camera);

void renderObject(Renderer *renderer, GameObject *object) ;

void freeRenderer(Renderer *renderer) ;

void getModelViewProj(Camera *camera, GameObject *object, mat4 dest) ;