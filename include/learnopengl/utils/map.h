#pragma once

#include <assert.h>
#include <limits.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <uchar.h>

#define FNV_PRIME 16777619
#define FNV_OFFSET_BASIS 2166136261
// get relatively prime for hash 2
#define MAX_LOAD_FACTOR 0.75
#define MAX_DEL_FACTOR 0.25

#define MARKER_EMPTY 0
#define MARKER_FULL 1
#define MARKER_DELETED 2

#define RESULT_SUCCESS 0
#define RESULT_ERROR -1
#define MAX_PROBES_EXCEEDED -2
#define RESULT_MALLOC_FAILED -3

#define FUNC_QUALIFIERS static inline
#define FUNC_TYPE(T) FUNC_QUALIFIERS T
#define FUNC_NAME(prefix, K_name, V_name) prefix##_##K_name##_##V_name

// make eq wokr on any struct by comparing bytes
#define size_name(T_name) size_of_c##T_name
#define SIZE_OF_C(T, n)                                                        \
  static inline size_t size_name(n)(T t) { return sizeof(T); }

SIZE_OF_C(float, f);
SIZE_OF_C(uint, uint);
SIZE_OF_C(char *, str);

#define eq_name(T_name) eq_c_##T_name
#define EQ_C(T, T_name)                                                        \
  static inline bool eq_name(T_name)(T x, T y) { return x == y; };

EQ_C(float, f);
EQ_C(uint, uint);
EQ_C(char *, str);

#define NEG_STRCMP                                                             \
  static inline bool neg_strcmp(char *x, char *y) { return !strcmp(x, y); }
NEG_STRCMP

#define SIZE_OF(X, T_name)                                                     \
  _Generic(X, char *: strlen, default: size_name(T_name))(X)
#define EQ(X, Y, T_name)                                                       \
  _Generic(X, char *: neg_strcmp, default: eq_name(T_name))(X, Y)

static inline uint fnv1a(char *data, size_t length) {
  uint hash = FNV_OFFSET_BASIS;
  for (int i = 0; i < length; i++) {
    hash ^= data[i];
    hash *= FNV_PRIME;
  }
  return hash;
}

static inline uint horners_str(char *data, size_t length) {
  uint hash = data[0];
  for (int i = 1; i < length; i++) {
    hash *= 31;
    hash += data[i];
  }
  return hash;
}

#define MAP(K, V)                                                              \
  struct {                                                                     \
    uint size;                                                                 \
    uint numDeleted;                                                           \
    uint capacity;                                                             \
    V *vals;                                                                   \
    K *keys;                                                                   \
    u_char *state;                                                             \
  }

#define _freeMap(M, K, V, K_name, V_name)                                      \
  FUNC_TYPE(void) FUNC_NAME(freeMap, K_name, V_name)(M * map) {                \
    if (map->keys) {                                                           \
      free(map->keys);                                                         \
      free(map->vals);                                                         \
      free(map->state);                                                        \
    }                                                                          \
  }

#define _reallocate(M, K, V, K_name, V_name)                                   \
  FUNC_TYPE(uint)                                                              \
  FUNC_NAME(reallocate, K_name, V_name)(M * map, uint newCapacity) {           \
    K *keys_p = (K *)malloc(sizeof(K) * newCapacity);                          \
    V *values_p = (V *)malloc(sizeof(V) * newCapacity);                        \
    u_char *state_p = (u_char *)malloc(sizeof(u_char) * newCapacity);               \
    if (!keys_p || !values_p || !state_p)                                         \
      return RESULT_MALLOC_FAILED;                                             \
    map->keys = keys_p;                                                        \
    map->vals = values_p;                                                      \
    map->state = state_p;                                                      \
    map->capacity = newCapacity;                                               \
    memset(map->state, MARKER_EMPTY, map->capacity);                           \
    return RESULT_SUCCESS;                                                     \
  }

#define _initHash(M, K, V, K_name, V_name)                                     \
  FUNC_TYPE(uint)                                                              \
  FUNC_NAME(initHash, K_name, V_name)(M * map, uint capacityExp) {             \
    map->numDeleted = 0;                                                       \
    map->size = 0;                                                             \
    map->keys = NULL;                                                          \
    map->vals = NULL;                                                          \
    map->state = NULL;                                                         \
    uint result_alloc =                                                        \
        FUNC_NAME(reallocate, K_name, V_name)(map, 1 << capacityExp);          \
    if (!result_alloc)                                                         \
      return result_alloc;                                                     \
    return RESULT_SUCCESS;                                                     \
  }

#define _rehash(M, K, V, K_name, V_name)                                       \
  FUNC_TYPE(uint)                                                              \
  FUNC_NAME(rehash, K_name, V_name)(M * map, uint newCapacity) {               \
    K *oldKeys = map->keys;                                                    \
    V *oldValues = map->vals;                                                  \
    u_char *oldState = map->state;                                             \
    uint oldCapacity = map->capacity;                                          \
    FUNC_NAME(reallocate, K_name, V_name)(map, newCapacity);                   \
    for (int i = 0; i < oldCapacity; i++) {                                    \
      if (oldState[i] != MARKER_FULL)                                          \
        continue;                                                              \
      FUNC_NAME(_put, K_name, V_name)(map, oldKeys[i], oldValues[i]);          \
    }                                                                          \
    free(oldKeys);                                                             \
    free(oldValues);                                                           \
    free(oldState);                                                            \
    map->numDeleted = 0;                                                       \
    return RESULT_SUCCESS;                                                     \
  }

#define _resize(M, K, V, K_name, V_name)                                       \
  FUNC_TYPE(uint) FUNC_NAME(resize, K_name, V_name)(M * map) {                 \
    uint newCapacity = map->capacity << 1;                                     \
    FUNC_NAME(rehash, K_name, V_name)(map, newCapacity);                       \
    return RESULT_SUCCESS;                                                     \
  }

#define _get_hash_1_index(M, K, V, K_name, V_name)                             \
  FUNC_TYPE(uint)                                                              \
  FUNC_NAME(get_hash_1_index, K_name, V_name)(M * map, K key,                  \
                                              size_t *length_key) {            \
    *length_key = SIZE_OF(key, K_name);                                        \
    char *keyPtr = _Generic(key, char *: key, default: (char *)&key);          \
    uint hash_1 = fnv1a(keyPtr, *length_key);                                  \
    return hash_1 & (map->capacity - 1);                                       \
  }

#define _get_hash_2_index(M, K, V, K_name, V_name)                             \
  FUNC_TYPE(uint)                                                              \
  FUNC_NAME(get_hash_2_index, K_name, V_name)(M * map, K key, uint * hash_2,   \
                                              uint index, size_t length_key) { \
    char *keyPtr = _Generic(key, char *: key, default: (char *)&key);          \
    if (!(*hash_2))                                                            \
      *hash_2 = horners_str(keyPtr, length_key) | 1;                           \
    index += *hash_2;                                                          \
    index &= map->capacity - 1;                                                \
    return index;                                                              \
  }

#define __get(M, K, V, K_name, V_name)                                         \
  FUNC_TYPE(uint)                                                              \
  FUNC_NAME(_get, K_name, V_name)(M * map, K key, V * value, uint * found) {   \
    size_t length;                                                             \
    uint index =                                                               \
        FUNC_NAME(get_hash_1_index, K_name, V_name)(map, key, &length);        \
    int iterations = 0;                                                        \
    uint hash_2 = 0;                                                           \
    while (map->state[index] != MARKER_EMPTY && iterations < map->capacity) {  \
      if (map->state[index] == MARKER_FULL &&                                  \
          EQ(map->keys[index], key, K_name)) {                                 \
        break; /*found*/                                                       \
      }                                                                        \
      iterations++;                                                            \
      index = FUNC_NAME(get_hash_2_index, K_name, V_name)(map, key, &hash_2,   \
                                                          index, length);      \
      assert(hash_2); /*avoid infinie loop if hash_2 is 0, which should not    \
                         happen*/                                              \
    }                                                                          \
    if (iterations >= map->capacity)                                           \
      return MAX_PROBES_EXCEEDED;                                              \
                                                                               \
    if (map->state[index] != MARKER_FULL)                                      \
      return RESULT_ERROR;                                                     \
    if (value)                                                                 \
      *value = map->vals[index];                                               \
    if (found)                                                                 \
      *found = index;                                                          \
    return RESULT_SUCCESS;                                                     \
  }

#define _get_func(M, K, V, K_name, V_name)                                     \
  FUNC_TYPE(uint) FUNC_NAME(get, K_name, V_name)(M * map, K key, V * value) {  \
    return FUNC_NAME(_get, K_name, V_name)(map, key, value, NULL);             \
  }

#define _remove(M, K, V, K_name, V_name)                                       \
  FUNC_TYPE(uint)                                                              \
  FUNC_NAME(remove, K_name, V_name)(M * map, K key, V * value) {               \
    uint result, index;                                                        \
    result = FUNC_NAME(_get, K_name, V_name)(map, key, value, &index);         \
    if (result)                                                                \
      return result;                                                           \
    map->state[index] = MARKER_DELETED;                                        \
    map->size--;                                                               \
    map->numDeleted++;                                                         \
    if (map->numDeleted > MAX_DEL_FACTOR * map->capacity) {                    \
      FUNC_NAME(rehash, K_name, V_name)(map, map->capacity);                   \
    }                                                                          \
    return RESULT_SUCCESS;                                                     \
  }

#define __put_internal(M, K, V, K_name, V_name)                                \
  FUNC_TYPE(uint) FUNC_NAME(_put, K_name, V_name)(M * map, K key, V value) {   \
    size_t length;                                                             \
    uint index =                                                               \
        FUNC_NAME(get_hash_1_index, K_name, V_name)(map, key, &length);        \
                                                                               \
    int iterations = 0;                                                        \
    int first_deleted = -1;                                                    \
    uint hash_2 = 0;                                                           \
    while (map->state[index] != MARKER_EMPTY && iterations < map->capacity) {  \
      if (map->state[index] == MARKER_FULL &&                                  \
          EQ(key, map->keys[index], K_name))                                   \
        goto success;                                                          \
                                                                               \
      if (map->state[index] == MARKER_DELETED)                                 \
        first_deleted = index;                                                 \
                                                                               \
      iterations++;                                                            \
      index = FUNC_NAME(get_hash_2_index, K_name, V_name)(map, key, &hash_2,   \
                                                          index, length);      \
      assert(hash_2);                                                          \
    }                                                                          \
    /* if max collisions reached */                                            \
    if (map->state[index] == MARKER_FULL)                                      \
      return MAX_PROBES_EXCEEDED;                                              \
                                                                               \
    if (first_deleted != -1) {                                                 \
      index = first_deleted;                                                   \
    }                                                                          \
                                                                               \
    map->size++;                                                               \
    goto success;                                                              \
  success:                                                                     \
    map->state[index] = MARKER_FULL;                                           \
    map->keys[index] = key;                                                    \
    map->vals[index] = value;                                                  \
                                                                               \
    return RESULT_SUCCESS;                                                     \
  }

#define put_func(M, K, V, K_name, V_name)                                      \
  FUNC_TYPE(uint) FUNC_NAME(put, K_name, V_name)(M * map, K key, V value) {    \
    uint result = FUNC_NAME(_put, K_name, V_name)(map, key, value);            \
    if (map->size >= MAX_LOAD_FACTOR * map->capacity) {                        \
      FUNC_NAME(resize, K_name, V_name)(map);                                  \
    }                                                                          \
    return RESULT_SUCCESS;                                                     \
  }

typedef MAP(char *, uint) HashMap_str_uint;
static inline void freeMap_str_uint(HashMap_str_uint *map) {
  for (int i = 0; i < map->capacity; i++) {
    if (map->state[i] == MARKER_FULL)
      free(map->keys[i]);
  }
  if (map->keys) {
    free(map->keys);
    free(map->vals);
    free(map->state);
  }
};
_reallocate(HashMap_str_uint, char *, uint, str, uint);
_initHash(HashMap_str_uint, char *, uint, str, uint);
_get_hash_1_index(HashMap_str_uint, char *, uint, str, uint);
_get_hash_2_index(HashMap_str_uint, char *, uint, str, uint);
static inline uint _put_str_uint(HashMap_str_uint *map, char *key, uint value) {
  size_t length;
  uint index = get_hash_1_index_str_uint(map, key, &length);
  int iterations = 0;
  int first_deleted = -1;
  uint hash_2 = 0;
  while (map->state[index] != 0 && iterations < map->capacity) {
    if (map->state[index] == 1 &&
        _Generic(key, char *: neg_strcmp, default: eq_c_str)(key,
                                                             map->keys[index]))
      goto success;
    if (map->state[index] == 2)
      first_deleted = index;
    iterations++;
    index = get_hash_2_index_str_uint(map, key, &hash_2, index, length);
    assert(hash_2);
  }
  if (map->state[index] == 1)
    return -2;
  if (first_deleted != -1) {
    index = first_deleted;
  }
  map->size++;
  goto success;
success:
  if (map->state[index] != MARKER_FULL) {
    char *p = malloc(strlen(key) + 1);
    if (!p)
      return RESULT_MALLOC_FAILED;
    map->keys[index] = p;
    strcpy(p, key);
  }
  map->state[index] = 1;
  map->vals[index] = value;
  return 0;
};
static inline uint rehash_str_uint(HashMap_str_uint *map, uint newCapacity) {
  char **oldKeys = map->keys;
  uint *oldValues = map->vals;
  u_char *oldState = map->state;
  uint oldCapacity = map->capacity;
  reallocate_str_uint(map, newCapacity);
  for (int i = 0; i < oldCapacity; i++) {
    if (oldState[i] != MARKER_FULL)
      continue;
    _put_str_uint(map, oldKeys[i], oldValues[i]);
    free(oldKeys[i]);
  }
  free(oldKeys);
  free(oldValues);
  free(oldState);
  map->numDeleted = 0;
  return 0;
};
_resize(HashMap_str_uint, char *, uint, str, uint);
static inline uint put_str_uint(HashMap_str_uint *map, char *key, uint value) {

  uint result = _put_str_uint(map, key, value);
  if (map->size >= MAX_LOAD_FACTOR * map->capacity) {
    resize_str_uint(map);
  }
  return RESULT_SUCCESS;
};
__get(HashMap_str_uint, char *, uint, str, uint);
static inline uint get_str_uint(HashMap_str_uint *map, char *key, uint *value) {
  return _get_str_uint(map, key, value, ((void *)0));
};
static inline uint remove_str_uint(HashMap_str_uint *map, char *key,
                                   uint *value) {
  uint result, index;
  result = _get_str_uint(map, key, value, &index);
  if (result)
    return result;
  map->state[index] = MARKER_DELETED;
  map->size--;
  free(map->keys[index]);
  map->numDeleted++;
  if (map->numDeleted > MAX_DEL_FACTOR * map->capacity) {
    uint result_alloc = FUNC_NAME(rehash, str, uint)(map, map->capacity);
    if (!result_alloc)
      return result_alloc;
  }
  return RESULT_SUCCESS;
};

typedef MAP(uint, char *) HashMap_uint_str;
_freeMap(HashMap_uint_str, uint, char *, uint, str);
_reallocate(HashMap_uint_str, uint, char *, uint, str);
_initHash(HashMap_uint_str, uint, char *, uint, str);
_get_hash_1_index(HashMap_uint_str, uint, char *, uint, str);
_get_hash_2_index(HashMap_uint_str, uint, char *, uint, str);
__put_internal(HashMap_uint_str, uint, char *, uint, str);
_rehash(HashMap_uint_str, uint, char *, uint, str);
_resize(HashMap_uint_str, uint, char *, uint, str);
put_func(HashMap_uint_str, uint, char *, uint, str);
__get(HashMap_uint_str, uint, char *, uint, str);
_get_func(HashMap_uint_str, uint, char *, uint, str);
_remove(HashMap_uint_str, uint, char *, uint, str);

typedef MAP(uint, uint) HashMap_uint_uint;
_freeMap(HashMap_uint_uint, uint, uint, uint, uint);
_reallocate(HashMap_uint_uint, uint, uint, uint, uint);
_initHash(HashMap_uint_uint, uint, uint, uint, uint);
_get_hash_1_index(HashMap_uint_uint, uint, uint, uint, uint);
_get_hash_2_index(HashMap_uint_uint, uint, uint, uint, uint);
__put_internal(HashMap_uint_uint, uint, uint, uint, uint);
_rehash(HashMap_uint_uint, uint, uint, uint, uint);
_resize(HashMap_uint_uint, uint, uint, uint, uint);
put_func(HashMap_uint_uint, uint, uint, uint, uint);
__get(HashMap_uint_uint, uint, uint, uint, uint);
_get_func(HashMap_uint_uint, uint, uint, uint, uint);
_remove(HashMap_uint_uint, uint, uint, uint, uint);
