#include <stdio.h>
#include <stdlib.h>
#include <math.h>

typedef double  f64;
typedef int     s32; 
typedef char    u8;

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

///
/// \brief  generate a random floating number between -180.0 and 180.0
///
static f64
generate_random_coordinate ()
{
  s32 random_number = rand();

  f64 factor = (f64)random_number / (f64)RAND_MAX;

  f64 result = -180.0 + (360 * factor);
  return result;
}


s32 
main (s32 argc, u8** argv)
{
  s32 output_ammount = 0;
  s32 seed = 0;
  f64 haversine_sum = 0;

  if (argc >= 2)
    {
      u8 *parameter = argv[1];
      output_ammount = atoi(parameter);
    }

  if (argc >= 3)
    {
      u8 *parameter = argv[2];
      seed = atoi(parameter);
    }

  srand(seed);

  FILE *fp = fopen("output.json", "w");
  if (fp)
    {
      fprintf(fp, "{\n");
      fprintf(fp, "\t\"pairs\": [\n");
      for (int i = 1; i <= output_ammount; i++)
        {
          f64 lon1 = generate_random_coordinate();
          f64 lon2 = generate_random_coordinate();
          f64 lat1 = generate_random_coordinate();
          f64 lat2 = generate_random_coordinate();

          fprintf(fp, "\t\t{");
          fprintf(fp, "\"x0\":%.10f, ", lon1);
          fprintf(fp, "\"y0\":%.10f, ", lat1);
          fprintf(fp, "\"x1\":%.10f, ", lon2);
          fprintf(fp, "\"y1\":%.10f  ", lat2);
          fprintf(fp, "}");

          haversine_sum += reference_haversine(lon1, lat1, lon2, lat2, EARTH_RADIUS);

          if (i != output_ammount)
            {
              fprintf(fp, ",");
            }

          fprintf(fp, "\n");
        }

      fprintf(fp, "\t]\n");
      fprintf(fp, "}\n");
    }

  fclose(fp);

  printf("sum: %f\n", haversine_sum);
  printf("avg: %f\n", (haversine_sum/output_ammount));


  return 0;
}
