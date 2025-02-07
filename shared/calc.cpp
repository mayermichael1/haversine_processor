#include "calc.h"
#include "stdio.h"

static f32 max_pow = -MAXFLOAT;
static f32 min_pow = MAXFLOAT;

static f32 max_sin = -MAXFLOAT;
static f32 min_sin = MAXFLOAT;

static f32 max_cos = -MAXFLOAT;
static f32 min_cos= MAXFLOAT;

static f32 max_asin = -MAXFLOAT;
static f32 min_asin = MAXFLOAT;

static f32 max_sqrt = -MAXFLOAT;
static f32 min_sqrt = MAXFLOAT;

inline f64 
mypow (f32 x, f32 y)
{
    f64 result = 0;
    if (x < min_pow)
    {
        min_pow = x;
    }
    else if (x > max_pow)
    {
        max_pow = x;
    }

    result = pow(x, y);
    return result;
}

inline f64 
mysin (f32 x)
{
    f64 result = 0;
    if (x < min_sin)
    {
        min_sin = x;
    }
    else if (x > max_sin)
    {
        max_sin = x;
    }

    result = sin(x);
    return result;
}

inline f64 
mycos (f32 x)
{
    f64 result = 0;
    if (x < min_cos)
    {
        min_cos = x;
    }
    else if (x > max_cos)
    {
        max_cos = x;
    }

    result = cos(x);
    return result;
}

inline f64 
myasin (f32 x)
{
    f64 result = 0;
    if (x < min_asin)
    {
        min_asin = x;
    }
    else if (x > max_asin)
    {
        max_asin = x;
    }

    result = asin(x);
    return result;
}

inline f64 
mysqrt (f32 x)
{
    f64 result = 0;
    if (x < min_sqrt)
    {
        min_sqrt = x;
    }
    else if (x > max_sqrt)
    {
        max_sqrt = x;
    }

    result = sqrt(x);
    return result;
}

f64 
reference_haversine ( coordinate coord1, coordinate coord2, f64 earth_radius)
{
    f64 delta_lat = degrees_to_radians(coord2.latitude - coord1.latitude); 
    f64 delta_lon = degrees_to_radians(coord2.longitude - coord1.longitude);

    coord1.latitude = degrees_to_radians(coord1.latitude);
    coord2.latitude = degrees_to_radians(coord2.latitude);

    f64 a = mypow(mysin(delta_lat/2.0), 2.0) + 
            mycos(coord1.latitude) * mycos(coord2.latitude) * 
            mypow(mysin(delta_lon/2.0), 2.0); 

    f64 c = 2.0 * myasin(mysqrt(a));

    f64 result = earth_radius * c;

    return result;
}

void
print_math_function_domains ()
{
    printf("sin : \tmin: %f\tmax: %f\n", min_sin, max_sin);
    printf("cos : \tmin: %f\tmax: %f\n", min_cos, max_cos);
    printf("pow : \tmin: %f\tmax: %f\n", min_pow, max_pow);
    printf("sqrt: \tmin: %f\tmax: %f\n", min_sqrt, max_sqrt);
    printf("asin: \tmin: %f\tmax: %f\n", min_asin, max_asin);
}
