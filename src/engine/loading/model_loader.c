#include "learnopengl/engine/mesh.h"
#include <assert.h>
#include <cglm/struct/vec2.h>
#include <cglm/struct/vec3.h>
#include <cglm/vec3.h>
#include <learnopengl/engine/loading/model_loader.h>
#include <learnopengl/utils/arraylist.h>
#include <learnopengl/utils/map.h>
#include <learnopengl/utils/utils.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

const char *VERTEX_MARKER = "v";
const char *NORMAL_MARKER = "vn";
const char *TEXTURE_MARKER = "vt";
const char *FACE_MARKER = "f";

uint getTangents(ArrayList_vertex *vertices, ArrayList_i *indices);
uint decodeFacePoint(char *facePoint, Vertex *vertex, ArrayList_vec3 *normals,
                     ArrayList_vec3 *positions, ArrayList_vec2 *textureCoords) {
  // VERY IMPORTANT TO ZERO OUT
  // OTHERWISE THINGS LIKE tangetn and bitagnetns break
  memset(vertex, 0, sizeof(Vertex));

  uint posIndex, normalIndex, uvIndex;
  uint numScanned =
      sscanf(facePoint, "%d/%d/%d", &posIndex, &uvIndex, &normalIndex);
  assert(numScanned >= 1);
  if (numScanned != 3)
    numScanned = sscanf(facePoint, "%d//%d", &posIndex, &normalIndex);
  // handle negative indices
  posIndex = posIndex < 0 ? positions->length - posIndex : posIndex - 1;
  normalIndex =
      numScanned >= 2
          ? normalIndex < 0 ? normals->length - normalIndex : normalIndex - 1
          : INT_MIN;
  uvIndex = numScanned == 3
                ? uvIndex < 0 ? textureCoords->length - uvIndex : uvIndex - 1
                : INT_MIN;

  // assert(normalIndex >= 0 && normalIndex < normals->length);
  uint result = getAt_vec3(normals, normalIndex, &vertex->normal);
  // assert(posIndex >= 0 && posIndex < positions->length);
  result = getAt_vec3(positions, posIndex, &vertex->pos);
  assert(!result);
  // assert(uvIndex >= 0 && uvIndex < textureCoords->length);
  result = getAt_vec2(textureCoords, uvIndex, &vertex->texPos);

  return 0;
}

uint addTriangle(char *triangleStr[3], HashMap_str_uint *vertexToIndex,
                 ArrayList_vec3 *positions, ArrayList_vec3 *normals,
                 ArrayList_vec2 *textureCoords, ArrayList_i *elements,
                 ArrayList_vertex *vertices) {
  // store a hashmap of previous vector
  for (int i = 0; i < 3; i++) {
    uint index;
    int result = get_str_uint(vertexToIndex, triangleStr[i], &index);
    assert(result != MAX_PROBES_EXCEEDED);
    // check if vertics are unique
    if (result != RESULT_SUCCESS) {
      // if not add them to hashmp with an increasing index and add them to
      // array of vertex data
      index = vertices->length;
      Vertex vertex;
      decodeFacePoint(triangleStr[i], &vertex, normals, positions,
                      textureCoords);
      // decode it
      put_str_uint(vertexToIndex, triangleStr[i], index);
      append_vertex(vertices, vertex);
    }
    append_i(elements, index);
  }

  // store ordering
  return 0;
}

uint loadModelFromFile(const char *path, Mesh *mesh) {

  ArrayList_vertex vertices;
  initArrayList_vertex(&vertices, 10000, 100);
  ArrayList_i elementIndex;
  initArrayList_i(&elementIndex, 10000, 100);

  FILE *fd = fopen(path, "r");

  if (!fd)
    goto error;

  ArrayList_vec3 positions;
  initArrayList_vec3(&positions, 10000, 10000);

  ArrayList_vec3 normals;
  initArrayList_vec3(&normals, 10000, 10000);

  ArrayList_vec2 textureCoords;
  initArrayList_vec2(&textureCoords, 10000, 10000);

  HashMap_str_uint vertexToIndex;
  initHash_str_uint(&vertexToIndex, 16);

  ulong lineLength;
  while (!feof(fd)) {
    char *line = NULL;
    getline(&line, &lineLength, fd);
    char type[lineLength + 1];
    memset(type, 0, sizeof(type));
    uint scanned = sscanf(line, "%s", type);

    if (!strcmp(VERTEX_MARKER, type)) {
      float x, y, z;
      uint numScanned = sscanf(line, "%*s %f %f %f", &x, &y, &z);
      assert(numScanned == 3);
      append_vec3(&positions, (vec3s){x, y, z});
    }
    if (!strcmp(NORMAL_MARKER, type)) {
      float x, y, z;
      uint numScanned = sscanf(line, "%*s %f %f %f", &x, &y, &z);
      assert(numScanned == 3);
      append_vec3(&normals, (vec3s){x, y, z});
    }
    if (!strcmp(TEXTURE_MARKER, type)) {
      float u, v;
      uint numScanned = sscanf(line, "%*s %f %f", &u, &v);
      assert(numScanned == 2);
      append_vec2(&textureCoords, (vec2s){u, v});
    }
    if (!strcmp(FACE_MARKER, type)) {
      char face1[4 * (lineLength + 1)];
      char *face2 = face1 + (lineLength + 1);
      char *face3 = face1 + 2 * (lineLength + 1);
      char *face4 = face1 + 3 * (lineLength + 1);
      uint numFaces =
          sscanf(line, "%*s %s %s %s %s", face1, face2, face3, face4);
      assert(numFaces >= 3);
      char *triangle1[3] = {face1, face2, face3};
      if (numFaces >= 4) {
        // 4 faces
        // chose 2 sets of 3 points to make triangles
        // add the other triangles
        char *triangle2[3] = {face3, face4, face1};
        addTriangle(triangle2, &vertexToIndex, &positions, &normals,
                    &textureCoords, &elementIndex, &vertices);
      }
      // 3 faces or more
      addTriangle(triangle1, &vertexToIndex, &positions, &normals,
                  &textureCoords, &elementIndex, &vertices);
    }
    free(line);
  }

  getTangents(&vertices, &elementIndex);

  initMesh(mesh, vertices.ptr, vertices.length, elementIndex.ptr,
           elementIndex.length);

  destroyArrayList_vec3(&positions);
  destroyArrayList_vec2(&textureCoords);
  destroyArrayList_vec3(&normals);
  destroyArrayList_vertex(&vertices);
  destroyArrayList_i(&elementIndex);
  freeMap_str_uint(&vertexToIndex);
  fclose(fd);
  return 0;

error:
  return -1;
}

uint getTangents(ArrayList_vertex *vertices, ArrayList_i *indices) {

  for (int i = 0; i < indices->length; i += 3) {
    Vertex *v0 = vertices->ptr + indices->ptr[i];
    Vertex *v1 = vertices->ptr + indices->ptr[i + 1];
    Vertex *v2 = vertices->ptr + indices->ptr[i + 2];

    vec3s dPos1 = glms_vec3_sub(v1->pos, v0->pos);
    vec3s dPos2 = glms_vec3_sub(v2->pos, v0->pos);
    vec2s dUV1 = glms_vec2_sub(v1->texPos, v0->texPos);
    vec2s dUV2 = glms_vec2_sub(v2->texPos, v0->texPos);

    float r = 1 / (dUV1.x * dUV2.y - dUV1.y * dUV2.x);
    vec3s tangent, tempTangent, bitangent, tempBitangent;
    tangent = glms_vec3_scale(dPos1, dUV2.y);
    tempTangent = glms_vec3_scale(dPos2, dUV1.y);
    tangent = glms_vec3_sub(tangent, tempTangent);
    tangent = glms_vec3_scale(tangent, r);

    bitangent = glms_vec3_scale(dPos2, dUV1.x);
    tempBitangent = glms_vec3_scale(dPos1, dUV2.x);
    bitangent = glms_vec3_sub(bitangent, tempBitangent);
    bitangent = glms_vec3_scale(bitangent, r);

    v0->biTangent = glms_vec3_add(v0->biTangent, bitangent);
    v0->tangent = glms_vec3_add(v0->tangent, tangent);

    v1->biTangent = glms_vec3_add(v1->biTangent, bitangent);
    v1->tangent = glms_vec3_add(v1->tangent, tangent);

    v2->biTangent = glms_vec3_add(v2->biTangent, bitangent);
    v2->tangent = glms_vec3_add(v2->tangent, tangent);
  }

  // removes need to normalize in shaders
  for (int i = 0; i < vertices->length; i++) {
    vertices->ptr[i].tangent = glms_normalize(vertices->ptr[i].tangent);
    vertices->ptr[i].biTangent = glms_normalize(vertices->ptr[i].biTangent);
  }

  return 0;
}
