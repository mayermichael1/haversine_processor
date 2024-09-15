#ifndef TYPES_H
#define TYPES_H

typedef double  f64;
typedef int     s32; 
typedef char    u8;
typedef bool    b8;
typedef float   f32;

#define EARTH_RADIUS    6372.8
#define CLUSTER_DIVISOR 1000
#define CLUSTER_RADIUS  7.5

struct coordinate
{
  f64 longitude;
  f64 latitude;
};

#endif
