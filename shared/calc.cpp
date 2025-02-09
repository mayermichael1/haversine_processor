#include "calc.h"
#include "stdio.h"
#include <float.h>

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


inline f64
square (f64 x)
{
    f64 result = 0;
    result = x * x;
    return result;
}

DOMAIN_CHECK(sin)
DOMAIN_CHECK(cos)
DOMAIN_CHECK(asin)
DOMAIN_CHECK(sqrt)

f64 
reference_haversine ( coordinate coord1, coordinate coord2, f64 earth_radius)
{
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
