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
#define MB KB * KB
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

static u8*
pre_fault_buffer (u8* file_name, u32 file_size, u32 buffer_size)
{
    u8* memory = (u8*)mmap(0, buffer_size, PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, 0, 0);
    for (u32 offset = 0; offset < buffer_size; offset += PAGE_SIZE)
    {
        memory[offset] = 0;
    }

    return memory;
}

static u8*
read_from_memory(u8* file_name, u32 file_size, u32 buffer_size, u8* read_buffer)
{
    u8* memory = (u8*)mmap(0, buffer_size, PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, 0, 0);

    s64 size_remaining = file_size;
    for (u64 offset = 0; offset < file_size; offset += buffer_size)
    {
        memcpy(memory, (read_buffer + offset), buffer_size);
        size_remaining -= offset; 
    }
    if (size_remaining > 0)
    {
        memcpy(memory, read_buffer, size_remaining);
    }

    return memory;
}


static u8*
read_from_file(u8* file_name, u32 file_size, u32 buffer_size)
{
    u8* memory = (u8*)mmap(0, buffer_size, PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, 0, 0);

    FILE *fp = fopen(file_name, "rb");
    s64 size_remaining = file_size;
    for (s64 offset = 0; offset < file_size; offset += buffer_size)
    {
        fread(memory, sizeof(u8), buffer_size, fp);
        size_remaining -= offset; 
    }
    fread(memory, sizeof(u8), buffer_size, fp);

    fclose(fp);

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

    if (argc >= 2)
    {
        json_file_name = argv[1];
    }

    cpu_frequency = estimate_cpu_frequencies();

    json_size = get_file_size(json_file_name);
    u8* read_buffer = (u8*)mmap(0, json_size, PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE , 0, 0);

    for (u32 buffer_size = 0.5 * KB; buffer_size < 4 * MB;)
    {
        buffer_size = buffer_size * 2;
        u8* memory = (u8*)mmap(0, buffer_size, PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE , 0, 0);

        printf("pre fault %i:\n", buffer_size);
        REPETITION_TEST_START(5.0);
        REPETITION_START_TIMER();
        pre_fault_buffer(json_file_name, json_size, buffer_size); 
        REPETITION_END_TIMER();
        REPETITION_TEST_END(cpu_frequency, json_size);
        printf("\n");

        printf("memory %i :\n", buffer_size);
        REPETITION_TEST_START(5.0);
        REPETITION_START_TIMER();
        read_from_memory(json_file_name, json_size, buffer_size, read_buffer); 
        REPETITION_END_TIMER();
        REPETITION_TEST_END(cpu_frequency, json_size);
        printf("\n");

        printf("file %i :\n", buffer_size);
        REPETITION_TEST_START(5.0);
        REPETITION_START_TIMER();
        read_from_file(json_file_name, json_size, buffer_size); 
        REPETITION_END_TIMER();
        REPETITION_TEST_END(cpu_frequency, json_size);
        printf("\n");
    }

    TIMED_BANDWITH("read",json_size);
    read_file(json_file_name, json_size);
    TIMED_BANDWITH_END("read");

#if 0
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

#endif

    TIMED_BLOCK_END("main");
    print_profiler();

    return 0;
}
