#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <limits.h>

#include "types.h"
#include "calc.h"
#include "profiler.h"

#define KB 1024
#define MB (KB * KB)
#define PAGE_SIZE 4096

static u64 cpu_frequency = 0;

static s32 
get_file_size (u8* file_name)
{
    FILE *fp = fopen(file_name, "rb");
    fseek(fp, 0, SEEK_END);
    s32 size = ftell(fp); 
    fclose(fp);
    return size;
}

static b8
check_haversine (f64 reference, f64 result)
{
    b8 correct = false;
    f64 epsilon = 0.000001f;

    f64 diff = result - reference;
    if (diff > -epsilon && diff < epsilon)
    {
        correct = true;
    }
    return correct;
}

static u8*
read_file (u8 *file_name, s32 file_size)
{
    //NOTE: this was used as the fastest option
    //      technically mapping with huge files would be faster but can not be 
    //      relied on
    u8* memory = NULL;
    //REPETITION_TEST_START(5.0f);
    memory = (u8*)mmap(0, file_size, PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE | MAP_POPULATE, 0, 0);

    FILE *fp = fopen(file_name, "rb");

    if (fp)
    {
        //REPETITION_START_TIMER();
        fread(memory, sizeof(u8), file_size, fp);
        //REPETITION_END_TIMER();

        fclose(fp);
    }

    //munmap(memory, file_size);
    //REPETITION_TEST_END(cpu_frequency);
    return memory;
}

s32
main (s32 argc, u8 **argv)
{
    init_page_fault_counter(); 

    TIMED_BLOCK("main");

    u8 *json_file_name = NULL;
    s32 json_size = 0;
    u8 *json_memory = NULL;

    u8 *results_file_name = NULL;
    s32 results_file_size = 0;
    f64 *results = NULL;

    s32 cursor = 0;
    s32 current_word_index = 0;
    u8 current_word[256] = "";

    u8 last_identifier[256] = "";
    s32 haversine_pairs_count = 0;
    f64 haversine_sum = 0;
    s32 amount_wrong = 0;

    haversine_pair *coordinate_store = (haversine_pair*)mmap(
        0, 
        sizeof(haversine_pair) * 10000000,
        PROT_WRITE,
        MAP_ANONYMOUS | MAP_PRIVATE | MAP_POPULATE,
        0,
        0
    );

    if (argc >= 2)
    {
        json_file_name = argv[1];
    }

    if (argc >= 3)
    {
        results_file_name = argv[2];
    }

    cpu_frequency = estimate_cpu_frequencies();

    json_size = get_file_size(json_file_name);
    TIMED_BANDWITH("read",json_size);
    json_memory = read_file(json_file_name, json_size);
    TIMED_BANDWITH_END("read");

    results_file_size = get_file_size(results_file_name);
    TIMED_BANDWITH("read_results",results_file_size);
    results = (f64*)read_file(results_file_name, results_file_size);
    TIMED_BANDWITH_END("read_results");


    TIMED_BLOCK("parse");
    haversine_pair pair = {0};
    while (cursor < json_size)
    {
        u8 character = *(json_memory+cursor);
        cursor++;
        current_word[current_word_index] = character;
        current_word_index++;

        switch (character)
            case ' ':
            case '\n':
            case '\r':
            case '\t':
            case ',':
            case '{':
            case '}':
            case '[':
            case ']':
            case ':':
            {
                current_word_index--;
                current_word[current_word_index] = 0;
                if (current_word[0] == '"' &&
                    current_word[current_word_index - 1] == '"')
                {
                    current_word[current_word_index - 1] = 0;
                    strcpy(last_identifier, &current_word[1]);
                }
                else 
                {
                    u8* endptr;
                    f64 value = strtod(current_word, &endptr); 
                    if (endptr != NULL &&
                        endptr != current_word)
                    {
                        if (strncmp(last_identifier, "x0", 2) == 0)
                        {
                            pair.coords[0].longitude = value;
                        }
                        else if (strncmp(last_identifier, "x1", 2) == 0)
                        {
                            pair.coords[1].longitude = value;
                        }
                        else if (strncmp(last_identifier, "y0", 2) == 0)
                        {
                            pair.coords[0].latitude = value;
                        }
                        else if (strncmp(last_identifier, "y1", 2) == 0)
                        {
                            pair.coords[1].latitude = value;

                            coordinate_store[haversine_pairs_count] = pair;
                            haversine_pairs_count++;
                            pair = {0};
                        }
                    }
                }
                current_word_index = 0;
                break;
            }
    }
    TIMED_BLOCK_END("parse");

    TIMED_BANDWITH("calc",haversine_pairs_count * sizeof(haversine_pair));
    for (u64 index = 0; index < haversine_pairs_count; ++index)
    {
        haversine_pair pair = coordinate_store[index];

        f64 haversine = reference_haversine(
            pair.coords[0],
            pair.coords[1],
            EARTH_RADIUS
        );

        amount_wrong += !check_haversine(*(results++), haversine);
        
        haversine_sum += haversine;
    }
    TIMED_BANDWITH_END("calc");
    f64 reference_sum_average = *results;

    f64 haversine_average = haversine_sum / (f64)haversine_pairs_count;
    printf("Processed average: %f\n", haversine_average);
    printf("Wrong haversine products: %i\n", amount_wrong);
    printf("Average Sum Error: %+34.24f\n", fabs(haversine_average - reference_sum_average));

    //print_math_function_domains();
    //

    // check different haversine implementations here
    printf("\nDifferent Haversine calculations compared here:\n");

    haversine_loop_test_function implementations[] = {
        {"haversine_reference", reference_haversine_loop},
        {"haversine_core_0", core_haversine_loop},
        {"haversine_core_1", core_1_haversine_loop},
        {"haversine_core_2", core_2_haversine_loop},
        {"haversine_core_3", core_3_haversine_loop},
        {"haversine_core_4", core_4_haversine_loop},
        {"haversine_core_5", core_5_haversine_loop},
        {"haversine_core_6", core_6_haversine_loop},
        {"haversine_core_7", core_7_haversine_loop},
        {"haversine_core_8", core_8_haversine_loop},
        {"haversine_core_9", core_8_haversine_loop},
        {"haversine_core_10", core_10_haversine_loop},
    };

    for(u32 i = 0; i < sizeof(implementations) / sizeof(haversine_loop_test_function); ++i)
    {
        haversine_loop_test_function implementation = implementations[i];
        TIMED_BANDWITH(implementation.function_name, haversine_pairs_count * sizeof(haversine_pair));
        f64 distance = implementation.function_call(coordinate_store, haversine_pairs_count);
        f64 error = fabs(distance - reference_sum_average);
        printf("%s:\tsum: %+34.24f\terror: %+34.24f\n",implementation.function_name, distance, error);
        TIMED_BANDWITH_END(implementation.function_name);
    }

    TIMED_BLOCK_END("main");
    printf("\n");
    print_profiler();

    return 0;
}
