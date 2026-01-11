
#include "camera.h"
#include "mesh.h"
#include "object.h"
#include <sys/types.h>

typedef struct{
    uint vao;
    uint vbo;
    uint ebo;
    Vertex* vertexBuffer;
    uint* elementBuffer;
    uint numVertices;
    uint maxVertexCount;
    uint numElements;
    uint maxElements;
    Camera* camera;
} BatchRenderer ;

void initBatchRenderer(BatchRenderer* renderer, uint maxElements, uint maxVertices);

void renderObject(BatchRenderer* renderer, GameObject* object);

void flush(BatchRenderer* renderer);
