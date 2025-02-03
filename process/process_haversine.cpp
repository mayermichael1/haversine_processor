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
    coordinate coords[2] = {};
    s32 haversine_calc_ammount = 0;
    f64 haversine_sum = 0;
    s32 amount_wrong = 0;

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
                            coords[0].longitude = value;
                        }
                        else if (strncmp(last_identifier, "x1", 2) == 0)
                        {
                            coords[1].longitude = value;
                        }
                        else if (strncmp(last_identifier, "y0", 2) == 0)
                        {
                            coords[0].latitude = value;
                        }
                        else if (strncmp(last_identifier, "y1", 2) == 0)
                        {
                            coords[1].latitude = value;
                            TIMED_BANDWITH("calc", 4 * sizeof(f64));
                            f64 haversine = reference_haversine(
                                coords[0],
                                coords[1],
                                EARTH_RADIUS
                            );

                            amount_wrong += !check_haversine(*(results++), haversine);
                            
                            haversine_sum += haversine;
                            TIMED_BANDWITH_END("calc");

                            haversine_calc_ammount++;
                            coords[0] = {0.0, 0.0};
                            coords[1] = {0.0, 0.0};
                        }
                    }
                }
                current_word_index = 0;
                break;
            }
    }
    TIMED_BLOCK_END("parse");

    printf("Processed average: %f\n", (haversine_sum/(f64)haversine_calc_ammount));
    printf("Wrong haversine products: %i\n", amount_wrong);

    TIMED_BLOCK_END("main");
    print_profiler();

    return 0;
}
