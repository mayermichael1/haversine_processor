#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

typedef double  f64;
typedef int     s32; 
typedef char    u8;

#define EARTH_RADIUS    6372.8
#define CLUSTER_DIVISOR 1000
#define CLUSTER_RADIUS  7.5

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

static f64
rand_0_to_1 ()
{
  f64 result = (f64)rand()/ (f64)RAND_MAX;
  return result;
}

///
/// \brief  generate a random floating number between -180.0 and 180.0
///
static f64
generate_random_coordinate (f64 coord = 0.0)
{
  f64 result = 0.0;
  f64 random = rand_0_to_1();

  if (coord == 0.0)
    {
      result = -180.0 + (360 * random);
    }
  else 
    {
      f64 delta = random * CLUSTER_RADIUS;
      f64 new_coord = coord + delta;

      if (new_coord > 180.0)
        {
          f64 overflow_delta = new_coord - 180.0;
          new_coord = -180.0 + overflow_delta;
        }
      else if (new_coord < -180.0)
        {
          f64 overflow_delta = new_coord - -180.0;
          new_coord = 180.0 + overflow_delta;
        }
      result = new_coord;
    }

  return result;
}

static void
get_random_cluster_coords (s32 cluster_ammount, f64 *cluster_lon_coords, f64 *cluster_lat_coords, f64 *lon, f64 *lat)
{
  f64 random = rand_0_to_1();
  s32 cluster_number = round(cluster_ammount * random);
  printf("%i / %i \n", cluster_number, cluster_ammount);
  *lon = cluster_lon_coords[cluster_number];
  *lat = cluster_lat_coords[cluster_number];
}

s32 
main (s32 argc, u8** argv)
{
  s32 output_ammount = 0;
  s32 seed = 0;
  f64 haversine_sum = 0;
  bool cluster = false;
  s32 cluster_ammount = 0;
  f64 *cluster_lat_coords;
  f64 *cluster_lon_coords;

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

  if (argc >= 4)
    {
      if (strcmp(argv[3], "-cluster") == 0)
        {
          cluster = true;
        }
    }

  srand(seed);

  if (cluster)
    {
      f64 random = rand_0_to_1();
      cluster_ammount = 1 + (output_ammount / CLUSTER_DIVISOR) * random;

      cluster_lat_coords = (f64*)malloc(sizeof(f64) * cluster_ammount);
      cluster_lon_coords = (f64*)malloc(sizeof(f64) * cluster_ammount);

      for (int i = 0; i < cluster_ammount; i++)
        {
          cluster_lon_coords[i] = generate_random_coordinate();
          cluster_lat_coords[i] = generate_random_coordinate();
        }
    }

  FILE *fp = fopen("output.json", "w");
  if (fp)
    {
      fprintf(fp, "{\n");
      fprintf(fp, "\t\"pairs\": [\n");
      for (int i = 1; i <= output_ammount; i++)
        {
          s32 cluster_number = 0;
          f64 cluster_lon = 0.0;
          f64 cluster_lat = 0.0;
          if (cluster)
            {
              get_random_cluster_coords (
                cluster_ammount, 
                cluster_lon_coords,
                cluster_lat_coords,
                &cluster_lon,
                &cluster_lat
              );
            }

          f64 lon1 = generate_random_coordinate(cluster_lon);
          f64 lat1 = generate_random_coordinate(cluster_lat);

          if (cluster)
            {
              get_random_cluster_coords (
                cluster_ammount, 
                cluster_lon_coords,
                cluster_lat_coords,
                &cluster_lon,
                &cluster_lat
              );
            }


          f64 lon2 = generate_random_coordinate(cluster_lon);
          f64 lat2 = generate_random_coordinate(cluster_lat);

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
