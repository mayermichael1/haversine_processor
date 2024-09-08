#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>

typedef double  f64;
typedef int     s32; 
typedef char    u8;

#define EARTH_RADIUS    6372.8
#define CLUSTER_DIVISOR 1000
#define CLUSTER_RADIUS  7.5

struct coordinate
{
  f64 longitude;
  f64 latitude;
};

coordinate 
operator+ (coordinate a, coordinate b)
{
  coordinate result = {};
  result.latitude = a.latitude + b.latitude;
  result.longitude = a.longitude + b.longitude;
  return result;
}

coordinate 
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
  return degree;
}


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

static f64
rand_0_to_1 ()
{
  f64 result = (f64)rand()/ (f64)RAND_MAX;
  return result;
}

///
/// \brief  generate a random floating number between -180.0 and 180.0
///
static coordinate
generate_random_coordinate (coordinate cluster_point = {})
{
  coordinate coord;
  f64 random = rand_0_to_1();

  if (cluster_point.latitude == 0.0 && cluster_point.longitude == 0.0)
    {
      coord.longitude = -180.0 + (360 * random);
      coord.latitude = -180.0 + (360 * random);
    }
  else 
    {
      f64 delta = random * CLUSTER_RADIUS;
      coordinate new_coord = cluster_point + delta;

      new_coord.longitude = keep_degrees_in_constraint(new_coord.longitude);
      new_coord.latitude = keep_degrees_in_constraint(new_coord.latitude);

      coord = new_coord;
    }

  return coord;
}

static void
get_random_cluster_coords (s32 cluster_ammount, coordinate *cluster_coords, coordinate *cluster)
{
  f64 random = rand_0_to_1();
  s32 cluster_number = round(cluster_ammount * random);
  cluster->longitude = cluster_coords[cluster_number].longitude;
  cluster->latitude = cluster_coords[cluster_number].latitude;
}

s32 
main (s32 argc, u8** argv)
{
  s32 output_ammount = 0;
  s32 seed = 0;
  f64 haversine_sum = 0;
  bool cluster = false;
  s32 cluster_ammount = 0;
  coordinate *cluster_coords;

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

  if (seed == 0)
    {
      seed = time(0);
    }
  srand(seed);

  if (cluster)
    {
      f64 random = rand_0_to_1();
      cluster_ammount = round(32 * random);

      cluster_coords = (coordinate*)malloc(sizeof(coordinate) * cluster_ammount);

      for (int i = 0; i < cluster_ammount; i++)
        {
          cluster_coords[i] = generate_random_coordinate({});
        }
    }

  FILE *fp = fopen("output.json", "w");
  if (fp)
    {
      fprintf(fp, "{\n");
      fprintf(fp, "\t\"pairs\": [\n");
      for (int i = 1; i <= output_ammount; i++)
        {
          coordinate cluster_coord = {};
          if (cluster)
            {
              get_random_cluster_coords (
                cluster_ammount, 
                cluster_coords,
                &cluster_coord
              );
            }

          coordinate coord1 = generate_random_coordinate(cluster_coord);

          if (cluster)
            {
              get_random_cluster_coords (
                cluster_ammount, 
                cluster_coords,
                &cluster_coord
              );
            }


          coordinate coord2 = generate_random_coordinate(cluster_coord);

          fprintf(fp, "\t\t{");
          fprintf(fp, "\"x0\":%.20f, ", coord1.longitude);
          fprintf(fp, "\"y0\":%.20f, ", coord1.latitude);
          fprintf(fp, "\"x1\":%.20f, ", coord2.longitude);
          fprintf(fp, "\"y1\":%.20f  ", coord2.latitude);
          fprintf(fp, "}");

          haversine_sum += reference_haversine(coord1, coord2, EARTH_RADIUS);

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
  if (cluster)
    {
      printf("clustered output\n");
    }


  return 0;
}
