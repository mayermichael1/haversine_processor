#include "calc.h"
#include "stdio.h"
#include <float.h>
#include "x86intrin.h"

#define DOMAIN_MIN_(name) min_ ##name
#define DOMAIN_MIN(name) DOMAIN_MIN_(name)

#define DOMAIN_MAX_(name) max_ ##name
#define DOMAIN_MAX(name) DOMAIN_MAX_(name)

#define EXPAND_MATH_FUNCTION(name, x) name(x)

#define DOMAIN_CHECK(name) \
    static f64 DOMAIN_MIN(name) = DBL_MAX; \
    static f64 DOMAIN_MAX(name) = -DBL_MAX; \
    f64 domain_##name (f64 x) { \
        f64 result = 0; \
        if (x < DOMAIN_MIN(name)){ DOMAIN_MIN(name) = x; } \
        else if (x > DOMAIN_MAX(name)){DOMAIN_MAX(name) = x;} \
        result = EXPAND_MATH_FUNCTION(name, x); \
        return result; \
    }

DOMAIN_CHECK(sin)
DOMAIN_CHECK(cos)
DOMAIN_CHECK(asin)
DOMAIN_CHECK(sqrt)

f64 
compare_math_implementations (f64 from, f64 to, f64 interval, 
    math_function_pointer reference, math_function_pointer compare_to)
{
    f64 max_error = 0;

    for (f64 i = from; i < to; i+=interval)
    {
        f64 difference = reference(i)  - compare_to(i);
        if (fabs(difference) > max_error)
        {
            max_error = fabs(difference);
        }
    }
    return max_error;
}
inline f64
square (f64 x)
{
    f64 result = 0;
    result = x * x;
    return result;
}


f64 
reference_haversine ( coordinate coord1, coordinate coord2, f64 earth_radius)
{
    f64 (*sinref)(f64 x) = sin;
    f64 delta_lat = degrees_to_radians(coord2.latitude - coord1.latitude); 
    f64 delta_lon = degrees_to_radians(coord2.longitude - coord1.longitude);

    f64 lat1 = degrees_to_radians(coord1.latitude);
    f64 lat2 = degrees_to_radians(coord2.latitude);

#if 1
    f64 a = square(domain_sin(delta_lat/2.0)) +
            domain_cos(lat1) * domain_cos(lat2) * square(domain_sin(delta_lon/2.0)); 

    f64 c = 2.0 * domain_asin(domain_sqrt(a));
#endif

#if 0
    f64 a = square(sin(delta_lat/2.0)) +
            cos(lat1) * cos(lat2) * square(sin(delta_lon/2.0)); 

    f64 c = 2.0 * asin(sqrt(a));

#endif

    f64 result = earth_radius * c;

    return result;
}

void
print_math_function_domains ()
{
    printf("sin : \tmin: %f\tmax: %f\n", min_sin, max_sin);
    printf("cos : \tmin: %f\tmax: %f\n", min_cos, max_cos);
    printf("sqrt: \tmin: %f\tmax: %f\n", min_sqrt, max_sqrt);
    printf("asin: \tmin: %f\tmax: %f\n", min_asin, max_asin);
}

f64 
my_sqrt (f64 x)
{
    f64 result = 0;
    __m128d wide_input = _mm_set_sd(x);
    __m128d wide_zero = _mm_set_sd(0);
    __m128d wide_result = _mm_sqrt_sd(wide_zero, wide_input);
    result = _mm_cvtsd_f64(wide_result);
    return result;
}

f64 
sin_half (f64 x)
{
    f64 abs_x = fabs(x);
    f64 result = (4 * abs_x) / PI - (4 * abs_x * abs_x) / (PI * PI);

    if (x < 0.0)
    {
        result = result * (-1);
    }
    return result;
}

f64 
sin_quarter (f64 x)
{
    f64 abs_x = fabs(x);
    if (abs_x > PI/2)
    {
        abs_x = PI/2 - (abs_x - PI/2);
    }
    f64 result = (4 * abs_x) / PI - (4 * abs_x * abs_x) / (PI * PI);

    if (x < 0.0)
    {
        result = result * (-1);
    }
    return result;
}

f64
cos_quarter (f64 x)
{
    x += PI/2;
    f64 result = sin_quarter(x);
    return result;
}

u64 
factorial (u64 x)
{
    u64 result = 1;
    for (u64 i = x; i > 0; --i)
    {
        result *= i;
    }
    return result;
}

f64
sin_taylor_series (f64 x, u8 factor)
{
    f64 result = x;
    f64 sign = -1;
    u8 current_factor = 3;
    while (current_factor < factor)
    {
        result += sign * pow(x, current_factor) / (f64)factorial(current_factor);
        current_factor += 2;
        sign *= -1;
    }
    return result;
}

f64
sin_taylor_7(f64 x)
{
    f64 abs_x = fabs(x);
    if (abs_x > PI/2)
    {
        abs_x = PI/2 - (abs_x - PI/2);
    }
    f64 result = sin_taylor_series(abs_x, 7);

    if (x < 0.0)
    {
        result = result * (-1);
    }
    return result;
}

f64
sin_taylor_9(f64 x)
{
    f64 abs_x = fabs(x);
    if (abs_x > PI/2)
    {
        abs_x = PI/2 - (abs_x - PI/2);
    }
    f64 result = sin_taylor_series(abs_x, 9);

    if (x < 0.0)
    {
        result = result * (-1);
    }
    return result;
}

f64
sin_taylor_11(f64 x)
{
    f64 abs_x = fabs(x);
    if (abs_x > PI/2)
    {
        abs_x = PI/2 - (abs_x - PI/2);
    }
    f64 result = sin_taylor_series(abs_x, 11);

    if (x < 0.0)
    {
        result = result * (-1);
    }
    return result;
}
