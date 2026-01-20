#pragma once
#include <math.h>
#include <limits.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <assert.h>

#define ArrayList(T) struct{ \
    uint capacity;  \
    uint growth;\
    uint length;\
    T* ptr;\
}

typedef ArrayList(char*) ArrayList_str ;
typedef ArrayList(float) ArrayList_f ;
typedef ArrayList(int) ArrayList_i;

#define ArrayList_t(T) ArrayList(T)

#define MAX(x,y)  x>=y ? x:y

#define FUNC_COMB(a,b) a##_##b

#define FUNC_MODS static inline

#define FUNC_TYPE(retT) FUNC_MODS retT

#define DELIN_NAME(name) name##_

#define INVALID_VAL(T) _Generic( T , \
    float : NAN,\
    int: INT_MAX,\
    char* : NULL,\
    default: NULL\
)



#define INIT_ARRAYLIST_FUNC(T) FUNC_COMB(initArrayList,T)
#define INSERT_DATA_AT_FUNC(T) FUNC_COMB(insertDataAt,T)
#define INSERT_AT_FUNC(T) FUNC_COMB(insertAt,T)
#define APPEND_FUNC(T) FUNC_COMB(append,T)
#define INSERT_LIST_AT_FUNC(T) FUNC_COMB(insertListAt,T)
#define EXTEND_FUNC(T) FUNC_COMB(extendData,T)
#define EXTEND_ARR_FUNC(T) FUNC_COMB(extendList,T)
#define GET_AT_FUNC(T)  FUNC_COMB(getAt,T)
#define REMOVE_AT_FUNC(T) FUNC_COMB(removeAt,T)
#define POP_FUNC(T) FUNC_COMB(pop,T)
#define REVERSE(T) FUNC_COMB(destroyArrayList,T)
#define DESTROY_FUNC(T) FUNC_COMB(destroyArrayList,T)
#define RESERVE_FUNC(T) FUNC_COMB(reserve,T)

#define initArrayList(A,T,S) FUNC_TYPE(uint) INIT_ARRAYLIST_FUNC(S) (A* a, int c, int g) { \
    if (c < 0) return -1; \
    if (g <= 0) return -1;\
    a->capacity = c; \
    a->length = 0; \
    a->growth = g; \
    a->ptr = c != 0? (T*)malloc(sizeof(T)*c) :(T*) NULL; \
    return 0; \
}

#define extendArrayList(A,T,S) FUNC_TYPE(uint) EXTEND_FUNC(S) (A* a, T* d, int l) { \
    return INSERT_DATA_AT_FUNC(S)(a, a->length>=1? a->length-1: 0, d , l); \
}
        //

#define insertDataArrayList(A,T,S) FUNC_TYPE(uint) INSERT_DATA_AT_FUNC(S) (A *arraylist, int offset, T *d, int l){ \
    if (offset + l > arraylist->capacity || arraylist->length + l > arraylist->capacity){ \
        uint newCapacity = (1+ (arraylist->length+l)/arraylist->growth) *arraylist->growth; \
        T * newPtr = (T*)malloc(sizeof(T)* newCapacity); \
        memcpy(newPtr, arraylist->ptr, offset*sizeof(T));\
        memcpy(newPtr+offset, d, l*sizeof(T));\
        memcpy(newPtr+offset+l, arraylist->ptr+offset, (arraylist->length-offset)*sizeof(T));\
        arraylist->capacity = newCapacity; \
        free(arraylist->ptr);\
        arraylist->ptr = newPtr;\
    } else{\
        memcpy(arraylist->ptr+offset+l, arraylist->ptr+offset, (arraylist->length-offset)*sizeof(T));\
        memcpy(arraylist->ptr+offset, d, l*sizeof(T));\
    }\
    arraylist->length += l;\
    return 0;\
}

#define extendArrayByArray(A,T,S) FUNC_TYPE(uint) EXTEND_ARR_FUNC(S) (A *arraylist, A* other){ \
   return EXTEND_FUNC(S)(arraylist, other->ptr, other->length);\
}
#define insertArrayList(A,T,S) FUNC_TYPE(uint) INSERT_AT_FUNC(S) (A *arraylist, uint index, T value){ \
   return INSERT_DATA_AT_FUNC(S)(arraylist, index,  &(value), 1);\
}
#define insertWithArray(A,T,S) FUNC_TYPE(uint) INSERT_LIST_AT_FUNC(S) (A *arraylist,  uint index, A* other){ \
   return INSERT_DATA_AT_FUNC(S)(arraylist, index,  other->ptr, other->length);\
}
# define appendArrayList(A,T,S) FUNC_TYPE(uint) APPEND_FUNC(S) (A* arraylist, T value){ \
    return INSERT_AT_FUNC(S)(arraylist, arraylist->length, value);\
}

#define getAtArrayList(A,T,S) FUNC_TYPE(T) GET_AT_FUNC(S) (A* arraylist, uint index){\
    assert(index >= 0 && index < arraylist->length);\
    return arraylist->ptr[index];\
}

#define removeAtArrayList(A,T,S) FUNC_TYPE(T) REMOVE_AT_FUNC(S) (A* arraylist, uint index ){\
    if (arraylist->length <= 0) return INVALID_VAL(T);\
    T value = GET_AT_FUNC(S)(arraylist,  index);\
    memcpy(arraylist->ptr+index, arraylist->ptr+index+1, sizeof(T)*(arraylist->length-index-1));\
    arraylist->length --;\
    return value;\
}

#define popArrayList(A,T,S) FUNC_TYPE(T) POP_FUNC(S) (A* arraylist){\
    return REMOVE_AT_FUNC(S)(arraylist, arraylist->length-1);\
}
#define destroyArrayList(A,T,S) FUNC_TYPE(void) DESTROY_FUNC(S) (A* arraylist){\
    if (arraylist->ptr) free(arraylist->ptr);\
}
#define reserveArrayList(A,T,S) FUNC_TYPE(uint) RESERVE_FUNC(S) (A* arraylist, uint newCapacity){\
    if (newCapacity < arraylist->capacity)  return -1;\
    if (arraylist->ptr) free(arraylist->ptr);\
    arraylist->capacity += newCapacity;\
    T* newPtr = (T*) malloc(sizeof(T)* arraylist->capacity );\
    memcpy(newPtr, arraylist->ptr, sizeof(T)*(arraylist->length));\
    arraylist->ptr = newPtr;\
    return 0;\
}

initArrayList(ArrayList_f,float, f);
insertDataArrayList(ArrayList_f, float, f);
extendArrayList(ArrayList_f, float, f);
extendArrayByArray(ArrayList_f, float, f);
insertArrayList(ArrayList_f, float, f);
insertWithArray(ArrayList_f, float, f)
appendArrayList(ArrayList_f, float,f );
getAtArrayList(ArrayList_f, float, f);
removeAtArrayList(ArrayList_f, float, f);
popArrayList(ArrayList_f, float, f)
destroyArrayList(ArrayList_f, float, f)
reserveArrayList(ArrayList_f, float, f);

initArrayList(ArrayList_i,int, i);
insertDataArrayList(ArrayList_i, int, i);
extendArrayList(ArrayList_i, int, i);
extendArrayByArray(ArrayList_i, int, i);
insertArrayList(ArrayList_i, int, i);
insertWithArray(ArrayList_i, int, i)
appendArrayList(ArrayList_i, int, i);
getAtArrayList(ArrayList_i, int, i);
removeAtArrayList(ArrayList_i, int, i);
popArrayList(ArrayList_i, int, i);
destroyArrayList(ArrayList_i, int,i);
reserveArrayList(ArrayList_i, int, i);

initArrayList(ArrayList_str,char *,str);
insertDataArrayList(ArrayList_str, char *,str);
extendArrayList(ArrayList_str, char *, str);
extendArrayByArray(ArrayList_str, char *, str);
insertArrayList(ArrayList_str, char *, str);
insertWithArray(ArrayList_str, char *, str)
appendArrayList(ArrayList_str, char *, str);
getAtArrayList(ArrayList_str, char *, str);
removeAtArrayList(ArrayList_str, char *, str);
popArrayList(ArrayList_str, char *, str);
destroyArrayList(ArrayList_str, char *,str);
reserveArrayList(ArrayList_str, char *, str);
