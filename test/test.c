#include <learnopengl/utils/arraylist.h>
#include "assert.h"
#include <math.h>
#include <string.h>

#define LENGTH(X) (uint)(sizeof(X)/sizeof(X[0]))

const uint GROWTH = 5;
const uint CAPACITY = 10;
const uint VALUE = 100;


ArrayList_f getInitVector(uint capacity, uint growth){
    ArrayList_f vector;
    uint ret =initArrayList_f(&vector, capacity, growth);
    assert(ret == 0);
    assert(vector.length == 0);
    assert(vector.capacity == capacity);
    return vector;
}

void extendVector(ArrayList_f *vector, float * data, uint length){
    uint prevLength = vector->length;
    uint  prevCap = vector->capacity;
    extendData_f(vector, data,  length);
    assert(vector->length == prevLength+ length);
    assert(length <= prevCap || vector->capacity == prevCap + vector->growth );
    assert(vector->length <= vector->capacity);
    assert(vector->ptr);
}

void testInitVector(){
    getInitVector(CAPACITY, GROWTH);
}

void testInitNegCapacity(){
    ArrayList_f vector;
    uint ret = initArrayList_f(&vector, -1, GROWTH);
    assert(ret);
}

void testInitNegOrZeroGrowth(){
    ArrayList_f vector;
    uint ret = initArrayList_f(&vector, -1,0);
    assert(!ret);
    ret = initArrayList_f(&vector, -1,-1);
    assert(!ret);
}

void testAppendEmpty(){
    ArrayList_f vector = getInitVector(CAPACITY, GROWTH);
    append_f(&vector, VALUE);
    assert(vector.ptr[0] == VALUE);
    assert(vector.length == 1);
}

void testPop(){
    ArrayList_f vector = getInitVector(CAPACITY, GROWTH);
    float arr[4] = {1,2,3,4};
    extendVector(&vector,arr,LENGTH(arr)) ;
    float poppedVal = pop_f(&vector);
    assert(poppedVal == arr[LENGTH(arr)-1]);
    assert(vector.length == LENGTH(arr)-1);
    assert(vector.ptr);
}

void testRemovingEmpty(){
    ArrayList_f vector = getInitVector(CAPACITY, GROWTH);
    float poppedVal = pop_f(&vector);
    assert(isnan(poppedVal));
    assert(vector.length == 0);
    assert(vector.ptr);
}

void testInsertEnd(){
    ArrayList_f vector = getInitVector(CAPACITY, GROWTH);
    float arr[9] = {1,2,3,4, 5,6,7,8,9};
    extendVector(&vector,arr, LENGTH(arr) );
    float* oldPtr = vector.ptr;

    insertAt_f(&vector,  LENGTH(arr), VALUE);
    assert(!memcmp(arr, vector.ptr,( CAPACITY-1)*sizeof(float)));
    assert(vector.ptr[vector.length-1] == VALUE);

    assert(oldPtr == vector.ptr);
    assert(vector.length == vector.capacity);
}

void testInsertStart(){
    ArrayList_f vector = getInitVector(CAPACITY, GROWTH);
    float arr[9] = {1,2,3,4, 5,6,7,8,9};
    extendVector(&vector,arr,LENGTH(arr));
    float* oldPtr = vector.ptr;
    insertAt_f(&vector, 0, VALUE);
    assert(!memcmp(arr, vector.ptr+1,LENGTH(arr)*sizeof(float)));
    assert(vector.ptr[0] == VALUE);
    assert(oldPtr == vector.ptr);
    assert(vector.length == vector.capacity);

}

void testInsertMiddle(){

    ArrayList_f vector = getInitVector(CAPACITY, GROWTH);
    float* oldPtr = vector.ptr;
    float arr[9] ={1,2,3,4, 5,6,7,8,9};
    extendVector(&vector,arr,CAPACITY-1);
    uint index = 5;
    insertAt_f(&vector, index,  VALUE);

    assert(!memcmp(arr, vector.ptr,( index)*sizeof(float)));
    assert(vector.ptr[index] == VALUE);
    assert(!memcmp(arr+index, vector.ptr+index+1,( CAPACITY-index-1 )*sizeof(float)));

    assert(oldPtr == vector.ptr);
    assert(vector.length == CAPACITY);

}

void testRemoveEnd(){
    ArrayList_f vector = getInitVector(CAPACITY, GROWTH);
    float* oldPtr = vector.ptr;
    float arr[10] = {1,2,3,4, 5,6,7,8,9, 10};
    extendVector(&vector,arr,CAPACITY);
    uint index = CAPACITY -1;
    float value = removeAt_f(&vector, index);
    assert(value == arr[index]);
    assert(vector.length == CAPACITY-1);
    assert(oldPtr == vector.ptr);
}

void testRemoveMiddle(){
    ArrayList_f vector = getInitVector(CAPACITY, GROWTH);
    float* oldPtr = vector.ptr;
    float arr[10] = {1,2,3,4, 5,6,7,8,9, 10};
    extendVector(&vector,arr,CAPACITY);
    uint index = 5;
    float value = removeAt_f(&vector, index);
    assert(value == arr[index]);
    assert(!memcmp(arr, vector.ptr,( index)*sizeof(float)));
    assert(!memcmp(arr+index+1, vector.ptr+index,( CAPACITY-index-1 )*sizeof(float)));
    assert(oldPtr == vector.ptr);

}


void testRemoveStart(){
    ArrayList_f vector = getInitVector(CAPACITY, GROWTH);
    float* oldPtr = vector.ptr;
    float newArr[10] = {1,2,3,4, 5,6,7,8,9, 10};
    extendVector(&vector,newArr,CAPACITY);
    uint index = 0;
    float value = removeAt_f(&vector, index);
    assert(value == newArr[index]);
    // assert(memcmp(newArr, vector.ptr,( index)*sizeof(float)));
    assert(!memcmp(newArr+index+1, vector.ptr+index,( CAPACITY-index-1 )*sizeof(float)));
    assert(oldPtr == vector.ptr);
    assert(vector.length == CAPACITY-1);
}

void testResizingAppend(){
    const uint capacity = 10;
    ArrayList_f vector = getInitVector(CAPACITY, GROWTH);
    float* oldPtr = vector.ptr;
    float newArr[10] = {1,2,3,4, 5,6,7,8,9, 10};
    extendVector(&vector,newArr,capacity);
    float value = 11;
    append_f(&vector,  value);
    assert(vector.ptr);
    assert(oldPtr != vector.ptr);
    assert(vector.ptr[CAPACITY] == value);
    assert(vector.length == capacity+1);
    assert(!memcmp(vector.ptr, newArr,( capacity)*sizeof(float)));
}

void testResizinginsertAt(){
    ArrayList_f vector = getInitVector(CAPACITY, GROWTH);
    float* oldPtr = vector.ptr;
    float newArr[10] = {1,2,3,4, 5,6,7,8,9, 10};
    extendVector(&vector,newArr,CAPACITY);
    uint index = 5;
    insertAt_f(&vector,  index, VALUE);
    assert(vector.ptr);
    assert(oldPtr != vector.ptr);
    assert(vector.length == CAPACITY+1);
    assert(!memcmp(vector.ptr, newArr,( index)*sizeof(float)));
    assert(vector.ptr[index] ==VALUE);
    assert(!memcmp(vector.ptr+index+1, newArr+index,( CAPACITY-index-1)*sizeof(float)));

}

void testResizingAddMultiple(){
    ArrayList_f vector = getInitVector(CAPACITY, GROWTH);
    float* oldPtr = vector.ptr;
    float arr[11] = {1,2,3,4, 5,6,7,8,9, 10, 11};
    extendVector(&vector,arr,CAPACITY+1);
    assert(oldPtr != vector.ptr);
    assert(!memcmp(vector.ptr, arr,( CAPACITY+1)*sizeof(float)));
    assert(vector.capacity == CAPACITY + GROWTH);
}

void testExtendAtResize(){
    ArrayList_f vector = getInitVector(CAPACITY, GROWTH);
    float* oldPtr = vector.ptr;
    float arr[] = {1,2,3,4, 5,6,7,8,9, 10};
    extendVector(&vector,arr,CAPACITY);
    float newArr [4] = { 20,30,40,50 } ;
    uint index = 5;
    insertDataAt_f(&vector, index ,newArr, LENGTH(newArr));
    assert(oldPtr != vector.ptr);

    assert(!memcmp(arr, vector.ptr ,(index)*sizeof(float)));
    assert(!memcmp(newArr, vector.ptr+index ,( LENGTH(newArr)*sizeof(float))));
    assert(!memcmp(vector.ptr+index+LENGTH(newArr), arr+index , (LENGTH(arr)-index)*sizeof(float)));

    assert(vector.length == LENGTH(newArr)+ CAPACITY);
    assert(vector.capacity == GROWTH+ CAPACITY);
}
void testExtendAtNOResize(){
    ArrayList_f vector = getInitVector(CAPACITY, GROWTH);
    float* oldPtr = vector.ptr;
    float arr[5] = {1,2,3,4, 5};
    extendVector(&vector,arr,LENGTH(arr));
    float newArr [4] = { 20,30,40,50 } ;
    uint index = 3;
    insertDataAt_f(&vector, index ,newArr, LENGTH(newArr));
    assert(oldPtr == vector.ptr);

    assert(!memcmp(arr, vector.ptr ,(index)*sizeof(float)));
    assert(!memcmp(newArr, vector.ptr+index ,( LENGTH(newArr)*sizeof(float))));
    assert(!memcmp(vector.ptr+index+LENGTH(newArr), arr+index , LENGTH(arr)-index)*sizeof(float));

    assert(vector.length == LENGTH(newArr)+ LENGTH(arr));
    assert(vector.capacity == CAPACITY);

}

void testExtendAtResizeVec(){
    ArrayList_f vector = getInitVector(CAPACITY, GROWTH);
    float* oldPtr = vector.ptr;
    const float arr[10] = {1,2,3,4, 5,6,7,8,9, 10};
    extendVector(&vector,arr,CAPACITY);
    ArrayList_f other = getInitVector(CAPACITY, GROWTH);
    float newArr [4] = { 20,30,40,50 } ;
    extendVector(&other,newArr,4);
    uint index = 5;
    insertListAt_f(&vector, index ,&other );
    assert(oldPtr != vector.ptr);

    assert(!memcmp(arr, vector.ptr ,(index)*sizeof(float)));
    assert(!memcmp(newArr, vector.ptr+index ,( LENGTH(newArr)*sizeof(float))));
    assert(!memcmp(vector.ptr+index+LENGTH(newArr), arr+index ,( LENGTH(arr)-index)*sizeof(float)));

    assert(vector.length == LENGTH(newArr)+ CAPACITY);
    assert(vector.capacity == GROWTH+ CAPACITY);
}
void testExtendAtNoResize(){
    ArrayList_f vector = getInitVector(CAPACITY, GROWTH);
    float* oldPtr = vector.ptr;
    float arr[5] = {1,2,3,4, 5};
    extendVector(&vector,arr,LENGTH(arr));
    float newArr [4] = { 20,30,40,50 } ;
    uint index = 3;
    insertDataAt_f(&vector, index ,newArr, LENGTH(newArr));
    assert(oldPtr == vector.ptr);

    assert(!memcmp(arr, vector.ptr ,(index)*sizeof(float)));
    assert(!memcmp(newArr, vector.ptr+index ,( LENGTH(newArr)*sizeof(float))));
    assert(!memcmp(vector.ptr+index+LENGTH(newArr), arr+index ,( LENGTH(arr)-index)*sizeof(float)));

    assert(vector.length == LENGTH(newArr)+ LENGTH(arr));
    assert(vector.capacity == CAPACITY);
}

void testExtendAtZeroCap(){
    ArrayList_f vector = getInitVector(0, GROWTH);
    float* oldPtr = vector.ptr;
    float arr[5] = {1,2,3,4, 5};
    insertDataAt_f(&vector,0,arr,LENGTH(arr));

    assert(!memcmp(arr, vector.ptr ,( LENGTH(arr)*sizeof(float))));

    assert(vector.length == LENGTH(arr));
    assert(vector.capacity == LENGTH(arr)+ GROWTH);
}

void testExtendZeroCap(){
    ArrayList_f vector = getInitVector(0, GROWTH);
    float* oldPtr = vector.ptr;
    float arr[5] = {1,2,3,4, 5};
    extendData_f(&vector,arr,LENGTH(arr));

    assert(!memcmp(arr, vector.ptr ,( LENGTH(arr)*sizeof(float))));

    assert(vector.length == LENGTH(arr));
    assert(vector.capacity == LENGTH(arr)+ GROWTH);
}

void testApendNoCap(){
    ArrayList_f vector = getInitVector(0, GROWTH);
    assert(!vector.ptr);
    append_f(&vector, VALUE );
    assert(vector.ptr);
    assert(vector.ptr[0] == VALUE);
    assert(vector.length == 1);
    assert(vector.capacity == GROWTH );
}

void testinsertNoCap(){
    ArrayList_f vector = getInitVector(0, GROWTH);
    assert(!vector.ptr);
    insertAt_f(&vector, 0 , VALUE);
    assert(vector.ptr);
    assert(vector.ptr[0] == VALUE);
    assert(vector.length == 1);
    assert(vector.capacity == GROWTH);
}

void testHugeInsertAt(){
    ArrayList_f vector = getInitVector(CAPACITY, GROWTH);
    float* oldPtr = vector.ptr;
    float arr[] = {1,2,3,4, 5,6,7,8,9, 10};
    extendVector(&vector,arr,CAPACITY);
    int huge_len = 103;
    float huge[huge_len];
    float value = 9001;
    memset(huge, *((int *)(&value)), sizeof(float)*103);
    int index = 3;
    insertDataAt_f(&vector, index, huge, huge_len);

    assert(oldPtr != vector.ptr);

    assert(!memcmp(arr, vector.ptr ,(index)*sizeof(float)));
    assert(!memcmp(huge, vector.ptr+index ,( LENGTH(huge)*sizeof(float))));
    assert(!memcmp(vector.ptr+index+LENGTH(huge), arr+index ,(LENGTH(arr)-index)*sizeof(float))  );

    assert(vector.length == LENGTH(huge)+ LENGTH(arr));
    assert(vector.capacity == 115);
}

void testReserve(){
      ArrayList_f vector = getInitVector(CAPACITY, GROWTH);
    float* oldPtr = vector.ptr;
    float arr[] = {1,2,3,4, 5,6,7,8,9, 10};
    extendVector(&vector,arr,CAPACITY);
    uint oldCap = vector.capacity;
    reserve_f(&vector, 100);
    assert(vector.ptr != oldPtr);
    assert(vector.capacity == oldCap + 100);
    assert(vector.length <= vector.capacity);
    assert(vector.length == CAPACITY);
}

void testReserveInvalid(){
      ArrayList_f vector = getInitVector(CAPACITY, GROWTH);
    float* oldPtr = vector.ptr;
    float arr[] = {1,2,3,4, 5,6,7,8,9, 10};
    extendVector(&vector,arr,CAPACITY);
    uint oldCap = vector.capacity;
    uint result = reserve_f(&vector, 5);
    assert(result);
    assert(vector.ptr == oldPtr);
    assert(vector.capacity == oldCap );
    assert(vector.length <= vector.capacity);
    assert(vector.length == CAPACITY);
}

int main(){
    testInitVector();
    testInitNegCapacity();
    testAppendEmpty();
    testPop();
    testRemovingEmpty();
    testInsertStart();
    testInsertMiddle();
    testInsertEnd();
    testRemoveStart();
    testRemoveMiddle();
    testRemoveEnd();
    testResizingAppend();
    testResizingAddMultiple();
    testResizinginsertAt();
    testApendNoCap();
    testinsertNoCap();
    testExtendAtNOResize();
    testExtendAtResize();
    testExtendAtResizeVec();
    testExtendZeroCap();
    testExtendAtZeroCap();
    testHugeInsertAt();
    testReserve();
    testReserveInvalid();
    return 0;
}
