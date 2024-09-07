#include <stdio.h>

#include <math.h>

typedef double f64;

#define EARTH_RADIUS 6372.8

static f64
degrees_to_radians (f64 degrees)
{
  f64 result = 0.01745329251994329577f * degrees;
  return result;
}

///
/// \brief  reference haversine calculation
///
static f64 
reference_haversine ( f64 lon1, f64 lat1, f64 lon2, f64 lat2, f64 earth_radius)
{
  f64 delta_lat = degrees_to_radians(lat2 - lat1); 
  f64 delta_lon = degrees_to_radians(lon2 - lon1);
  
  lat1 = degrees_to_radians(lat1);
  lat2 = degrees_to_radians(lat2);

  f64 a = pow(sin(delta_lat/2.0), 2.0) + 
          cos(lat1) * cos(lat2) * pow(sin(delta_lon/2.0), 2.0); 
  
  f64 c = 2.0 * asin(sqrt(a));

  f64 result = earth_radius * c;

  return result;
}

int 
main ()
{
  f64 haversine = reference_haversine(15.0, 14.0, 90.5, -2.0, EARTH_RADIUS);
  printf("haversine: %f \n", haversine);
  return 0;
}
