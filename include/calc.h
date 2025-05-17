#ifndef CALC_H
#define CALC_H

#define PI 3.14159265358979323846264338327950288419716939937510582097494459230781640628

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

void 
sin_taylor_test();

void 
sin_taylor_horner_test();

void 
sin_taylor_horner_fmadd_test();

void
sin_coefficient_array_test();

void 
asin_coefficient_array_test();

void 
asin_coefficient_array_test_extended();

void
test_core_functions();

///
/// \brief  reference haversine calculation
///
f64 
reference_haversine ( coordinate coord1, coordinate coord2, f64 earth_radius);

void
print_math_function_domains ();

f64
my_sqrt(f64 x);

// NOTE: this does only approximate in range -PI to PI
f64 
sin_half (f64 x);

// NOTE: this does only approximate in range -PI to PI
f64 
sin_quarter (f64 x);

// NOTE: this does only approximate in range -PI/2 to PI/2
f64 
cos_quarter (f64 x);

f64
sin_taylor_series (f64 x, u8 factor);

//f64 
//sin_coefficient_array(f64 x, f64 *coefficients, u8 count);

f64
asin_approximated(f64 x);


/// NOTE: 
/// these are the final math functions that will be used for haversine calculation
f64 
asin_core(f64 x); 

f64 
sqrt_core(f64 x);

#endif
