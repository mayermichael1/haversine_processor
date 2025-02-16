#ifndef CALC_H
#define CALC_H

#include <math.h>

#include "types.h"

inline coordinate 
operator+ (coordinate a, coordinate b)
{
    coordinate result = {};
    result.latitude = a.latitude + b.latitude;
    result.longitude = a.longitude + b.longitude;
    return result;
}

inline coordinate 
operator+ (coordinate a, f64 delta)
{
    coordinate result = {};
    result.latitude = a.latitude + delta;
    result.longitude = a.longitude + delta;
    return result;
}

inline f64 
keep_degrees_in_constraint (f64 degree)
{
    if (degree > 180.0)
    {
        f64 overflow_delta = degree - 180.0;
        degree = -180.0 + overflow_delta;
    }
    else if (degree < -180.0)
    {
        f64 overflow_delta = degree - -180.0;
        degree = 180.0 + overflow_delta;
    }
    assert(degree <= 180.0 && degree >= -180.0);
    return degree;
}


inline static f64
degrees_to_radians (f64 degrees)
{
    f64 result = 0.01745329251994329577f * degrees;
    return result;
}

typedef f64 (*math_function_pointer)(f64 x);

f64 
compare_math_implementations (f64 from, f64 to, f64 interval, 
    math_function_pointer reference, math_function_pointer compare_to);

///
/// \brief  reference haversine calculation
///
f64 
reference_haversine ( coordinate coord1, coordinate coord2, f64 earth_radius);

void
print_math_function_domains ();

f64 
my_sqrt (f64 x);

#endif
