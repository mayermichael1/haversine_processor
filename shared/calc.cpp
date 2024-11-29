#include "calc.h"

f64 
reference_haversine ( coordinate coord1, coordinate coord2, f64 earth_radius)
{
    f64 delta_lat = degrees_to_radians(coord2.latitude - coord1.latitude); 
    f64 delta_lon = degrees_to_radians(coord2.longitude - coord1.longitude);

    coord1.latitude = degrees_to_radians(coord1.latitude);
    coord2.latitude = degrees_to_radians(coord2.latitude);

    f64 a = pow(sin(delta_lat/2.0), 2.0) + 
            cos(coord1.latitude) * cos(coord2.latitude) * 
            pow(sin(delta_lon/2.0), 2.0); 

    f64 c = 2.0 * asin(sqrt(a));

    f64 result = earth_radius * c;

    return result;
}

