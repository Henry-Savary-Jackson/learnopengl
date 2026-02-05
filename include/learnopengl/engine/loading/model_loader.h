#pragma  once
#include "learnopengl/engine/mesh.h"
#include <learnopengl/utils/arraylist.h>
#include <cglm/types-struct.h>
#include <learnopengl/utils/map.h>
#include <sys/types.h>
#include <cglm/cglm.h>

typedef ArrayList(vec3s) ArrayList_vec3;
initArrayList(ArrayList_vec3,vec3s, vec3);
insertDataArrayList(ArrayList_vec3, vec3s, vec3);
extendArrayList(ArrayList_vec3, vec3s, vec3);
extendArrayByArray(ArrayList_vec3, vec3s, vec3);
insertArrayList(ArrayList_vec3, vec3s, vec3);
insertWithArray(ArrayList_vec3, vec3s, vec3)
appendArrayList(ArrayList_vec3, vec3s, vec3 );
getAtArrayList(ArrayList_vec3, vec3s, vec3);
removeAtArrayList(ArrayList_vec3, vec3s, vec3);
popArrayList(ArrayList_vec3, vec3s, vec3);
destroyArrayList(ArrayList_vec3, vec3s, vec3);
reserveArrayList(ArrayList_vec3, vec3s, vec3);

typedef ArrayList(vec2s) ArrayList_vec2;
initArrayList(ArrayList_vec2,vec2s, vec2);
insertDataArrayList(ArrayList_vec2, vec2s, vec2);
extendArrayList(ArrayList_vec2, vec2s, vec2);
extendArrayByArray(ArrayList_vec2, vec2s, vec2);
insertArrayList(ArrayList_vec2, vec2s, vec2);
insertWithArray(ArrayList_vec2, vec2s, vec2)
appendArrayList(ArrayList_vec2, vec2s, vec2 );
getAtArrayList(ArrayList_vec2, vec2s, vec2);
removeAtArrayList(ArrayList_vec2, vec2s, vec2);
popArrayList(ArrayList_vec2, vec2s, vec2);
destroyArrayList(ArrayList_vec2, vec2s, vec2);
reserveArrayList(ArrayList_vec2, vec2s, vec2);

typedef ArrayList(Vertex) ArrayList_vertex;
initArrayList(ArrayList_vertex,Vertex, vertex);
insertDataArrayList(ArrayList_vertex, Vertex, vertex);
extendArrayList(ArrayList_vertex, Vertex, vertex);
extendArrayByArray(ArrayList_vertex, Vertex, vertex);
insertArrayList(ArrayList_vertex, Vertex, vertex);
insertWithArray(ArrayList_vertex, Vertex, vertex)
appendArrayList(ArrayList_vertex, Vertex, vertex );
getAtArrayList(ArrayList_vertex, Vertex, vertex);
removeAtArrayList(ArrayList_vertex, Vertex, vertex);
popArrayList(ArrayList_vertex, Vertex, vertex);
destroyArrayList(ArrayList_vertex, Vertex, vertex);
reserveArrayList(ArrayList_vertex, Vertex, vertex);




uint loadModelFromFile(const char *path, Mesh* mesh);
