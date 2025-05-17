#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>

#include "types.h"
#include "calc.h"

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
        coord.longitude = -180.0 + (360.0 * random);
        coord.latitude = -90.0 + (180.0 * random);
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
    s32 cluster_number = round((cluster_ammount-1) * random);
    cluster->longitude = cluster_coords[cluster_number].longitude;
    cluster->latitude = cluster_coords[cluster_number].latitude;
}

static void
generate_random_clusters (s32 *cluster_ammount, coordinate **cluster_coords)
{      
    f64 random = rand_0_to_1();
    *cluster_ammount = round(32.f * random);

    *cluster_coords = (coordinate*)malloc(sizeof(coordinate) * (*cluster_ammount));

    for (int i = 0; i < (*cluster_ammount); i++)
    {
        (*cluster_coords)[i] = generate_random_coordinate();
    }
}

s32 
main (s32 argc, u8** argv)
{
    s32 output_ammount = 0;
    s32 seed = 0;
    f64 haversine_sum = 0;
    bool cluster = false;
    coordinate *cluster_coords;
    s32 cluster_ammount = 0;

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
        generate_random_clusters(&cluster_ammount, &cluster_coords);
    }

    FILE *fp = fopen("output.json", "w");
    FILE *fp_results = fopen("results.bin", "wb");
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

            if (i != output_ammount)
            {
                fprintf(fp, ",");
            }

            fprintf(fp, "\n");
            
            f64 result = reference_haversine(coord1, coord2, EARTH_RADIUS);
            haversine_sum += result;
            fwrite(&result, sizeof(result), 1, fp_results); 
        }

        fprintf(fp, "\t]\n");
        fprintf(fp, "}\n");
    }

    f64 average = haversine_sum / output_ammount;
    fwrite(&average, sizeof(average), 1, fp_results); 

    fclose(fp);
    fclose(fp_results);

    printf("sum: %f\n", haversine_sum);
    printf("avg: %f\n", average);

    if (cluster)
    {
        printf("clustered output\n");
    }


    return 0;
}
