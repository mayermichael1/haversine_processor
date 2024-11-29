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

#define PAGE_SIZE 4096

static u64 cpu_frequency = 0;

static void
fill_memory_with_zero(u64 count, u8* memory)
{
    for(u64 i = 0; i < count; i++)
    {
        memory[i] = 0;
    }
}

static void
fill_memory_with_random_bytes(u64 count, u8* memory)
{
    fill_memory_with_zero(count, memory);
    s32 random_fd = open("/dev/urandom", O_RDONLY);
    u64 bytes_written = 0;
    while (bytes_written < count)
    {
        u64 read_count = count - bytes_written;
        if (read_count > SSIZE_MAX)
        {
            read_count = SSIZE_MAX;
        }

        read(random_fd, memory, read_count);

        bytes_written += read_count;
    }
    close(random_fd);
}

static void
fill_memory_with_random_bytes_rand(u64 count, u8* memory)
{
    fill_memory_with_zero(count, memory);
    for(u64 i = 0; i < count; ++i)  
    {
        memory[i] = rand();
    }
}


static void
fill_memory_with_1_every_x(u64 count, u8* memory, u32 stride)
{
    fill_memory_with_zero(count, memory);
    for(u64 i = 0; i < count; i+=stride)
    {
        memory[i] = 1;
    }
}

static s32 
get_file_size (u8* file_name)
{
    FILE *fp = fopen(file_name, "rb");
    fseek(fp, 0, SEEK_END);
    s32 size = ftell(fp); 
    fclose(fp);
    return size;
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

static void
write_to_bytes (u64 bytes)
{
    u8* memory = (u8*)mmap(0, bytes, PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, 0, 0);

    for (u32 i = 0; i < bytes; i++)
    {
        *(memory+i) = i;
    }

    munmap(memory, bytes);
}

static bool
str_contains_any(u8* str, u8* characters)
{
    b8 result = false;
    u8* find_cursor = characters;
    u8 compare_character = 0;

    while ((compare_character = (*find_cursor++)) != 0)
    {
        u8* cursor = str;
        u8 character = 0;
        while ((character = (*cursor++)) != 0)
        {
            if (character == compare_character)
            {
                result = true;
            }
        }
    }
    return result;
}

s32
main (s32 argc, u8 **argv)
{
    init_page_fault_counter(); 

    TIMED_BLOCK("main");

    u8 *json_file_name = NULL;
    s32 json_size = 0;
    u8 *json_memory = NULL;

    s32 cursor = 0;
    s32 current_word_index = 0;
    u8 current_word[256] = "";

    u8 last_identifier[256] = "";
    coordinate coords[2] = {};
    s32 haversine_calc_ammount = 0;
    f64 haversine_sum = 0;

    /*
    cpu_frequency = estimate_cpu_frequencies();
    //
    u64 GB = 1024 * 1024 * 1024;
    TIMED_BANDWITH("write_bytes", GB * 3);
    write_to_bytes(GB * 3);
    TIMED_BANDWITH_END("write_bytes");
    //NOTE: this loop equates to 1.17 cycles per loop iteration

    u8* memory = (u8*)malloc(GB * 3);
    TIMED_BANDWITH("MOV", GB * 3);
    MOVAllBytesASM(GB * 3, memory);
    TIMED_BANDWITH_END("MOV");

    TIMED_BANDWITH("NOP", GB * 3);
    NOPAllBytesASM(GB * 3);
    TIMED_BANDWITH_END("NOP");

    TIMED_BANDWITH("CMP", GB * 3);
    CMPAllBytesASM(GB * 3);
    TIMED_BANDWITH_END("CMP");

    TIMED_BANDWITH("DEC", GB * 3);
    DECAllBytesASM(GB * 3);
    TIMED_BANDWITH_END("DEC");


    printf("frequency: %lu\n", cpu_frequency);

    */
    if (argc >= 2)
    {
        json_file_name = argv[1];
    }

    json_size = get_file_size(json_file_name);
    TIMED_BANDWITH("read",json_size);
    json_memory = read_file(json_file_name, json_size);
    TIMED_BANDWITH_END("read");

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
                            haversine_sum += reference_haversine(
                                coords[0],
                                coords[1],
                                EARTH_RADIUS
                            );
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

    TIMED_BLOCK_END("main");
    print_profiler();

    return 0;
}
