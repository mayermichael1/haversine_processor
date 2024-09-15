#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>

typedef double  f64;
typedef float   f32;

typedef char      u8;
typedef uint64_t  u64; 

typedef int       s32; 

typedef bool    b8;

#define EARTH_RADIUS    6372.8
#define CLUSTER_DIVISOR 1000
#define CLUSTER_RADIUS  7.5

struct coordinate
{
  f64 longitude;
  f64 latitude;
};

#endif
