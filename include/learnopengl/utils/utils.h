#pragma once

#define LENGTH(x) (uint)(sizeof(x)/sizeof(x[0]))

#define MAX(x,y)  x>=y ? x:y
#define MIN(x,y)  x>=y ? y:x

static inline char separator()
{
#ifdef _WIN32
    return '\\';
#else
    return '/';
#endif
}
