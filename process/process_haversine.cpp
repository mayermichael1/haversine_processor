#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>

#include "types.h"
#include "calc.h"
#include "profiler.h"

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
read_file_realloc (u8 *file_name, s32 file_size)
{
  u8* memory = NULL;
  REPETITION_TEST_START(5.0f);
  memory = (u8*)malloc(file_size);
  
  FILE *fp = fopen(file_name, "rb");

  //NOTE: differnce here: malloc is executed every iteration but not timed

  if (fp)
    {
      REPETITION_START_TIMER();
      fread(memory, sizeof(u8), file_size, fp);
      REPETITION_END_TIMER();

      fclose(fp);
    }

  free(memory);
  REPETITION_TEST_END(cpu_frequency);
  return memory;
}

static u8*
read_file_mmap (u8 *file_name, s32 file_size)
{
  u8* memory = NULL;
  REPETITION_TEST_START(5.0f);
  memory = (u8*)mmap(0, file_size, PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, 0, 0);
  
  FILE *fp = fopen(file_name, "rb");

  if (fp)
    {
      REPETITION_START_TIMER();
      fread(memory, sizeof(u8), file_size, fp);
      REPETITION_END_TIMER();

      fclose(fp);
    }

  munmap(memory, file_size);
  REPETITION_TEST_END(cpu_frequency);
  return memory;
}

static u8*
read_file_mmap_rounded_to_page (u8 *file_name, s32 file_size)
{
  u8* memory = NULL;
  REPETITION_TEST_START(5.0f);
  //s32 mask = ~(4095);
  s32 mask = ~(2097151);
  s32 file_size_rounded = (file_size + 2097151) & mask;
  memory = (u8*)mmap(0, file_size_rounded, PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE, 0, 0);

  assert(file_size_rounded %  2097152 == 0)

  FILE *fp = fopen(file_name, "rb");

  if (fp)
    {
      REPETITION_START_TIMER();
      fread(memory, sizeof(u8), file_size, fp);
      REPETITION_END_TIMER();

      fclose(fp);
    }

  munmap(memory, file_size_rounded);
  REPETITION_TEST_END(cpu_frequency);
  return memory;
}

static u8*
read_file_mmap_precommit (u8 *file_name, s32 file_size)
{
  u8* memory = NULL;
  REPETITION_TEST_START(5.0f);
  memory = (u8*)mmap(0, file_size, PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE | MAP_POPULATE, 0, 0);
  
  FILE *fp = fopen(file_name, "rb");

  //NOTE: differnce here: malloc is executed every iteration but not timed

  if (fp)
    {
      REPETITION_START_TIMER();
      fread(memory, sizeof(u8), file_size, fp);
      REPETITION_END_TIMER();

      fclose(fp);
    }

  munmap(memory, file_size);
  REPETITION_TEST_END(cpu_frequency);
  return memory;
}

static u8*
read_file_mmap_huge (u8 *file_name, s32 file_size)
{
  u8* memory = NULL;
  REPETITION_TEST_START(5.0f);
  memory = (u8*)mmap(0, file_size, PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE | MAP_HUGETLB | MAP_HUGE_2MB, 0, 0);
  
  FILE *fp = fopen(file_name, "rb");

  //NOTE: differnce here: malloc is executed every iteration but not timed

  if (fp)
    {
      REPETITION_START_TIMER();
      fread(memory, sizeof(u8), file_size, fp);
      REPETITION_END_TIMER();

      fclose(fp);
    }

  munmap(memory, file_size);
  REPETITION_TEST_END(cpu_frequency);
  return memory;
}

static void
read_file (u8 *file_name, s32 bytes_to_read, u8* memory)
{

  REPETITION_TEST_START(5.0f);
  FILE *fp = fopen(file_name, "rb");
  if (fp)
    {
      REPETITION_START_TIMER();
      fread(memory, sizeof(u8), bytes_to_read, fp);
      REPETITION_END_TIMER();

      fclose(fp);
    }
  REPETITION_TEST_END(cpu_frequency);
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


static void
probe_page_faults (u32 page_count)
{
  printf("probe_to; page_faults; difference\t");
  for (u32 probe_to = 1; probe_to <= page_count; ++probe_to)
    {
      u64 page_faults_before = get_page_fault_count();
      u8* memory = (u8*)mmap(0, page_count * PAGE_SIZE, PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);
      
      for (u32 probe_to_counter = 0;
           probe_to_counter < probe_to;
           ++probe_to_counter)
        {
          memory[PAGE_SIZE * probe_to_counter] = '1';
        }
      u64 page_faults_after = get_page_fault_count();
      u64 page_faults = page_faults_after - page_faults_before;

      munmap(memory, PAGE_SIZE * page_count);
      printf("%5u; %5lu; %5li;\n", probe_to, page_faults, (s64)(page_faults - probe_to));
    }
}

static void
test_large_alloc()
{
  u64 GB = 1024 * 1024 * 1024;
  u8* memory = (u8*)mmap(0, 50 * GB, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);

  if (memory == MAP_FAILED)
    {
      printf("Memory alloc did not work\n");
    }
  u8 in = '0';
  scanf(" %c", &in);
  printf("Now commiting 1GB of this memory\n");
  mmap(memory, GB, PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED, 0, 0);
  memory[0] = 'H';
  scanf(" %c", &in);
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

  cpu_frequency = estimate_cpu_frequencies();

  if (argc >= 2)
    {
      json_file_name = argv[1];
    }

  json_size = get_file_size(json_file_name);
  json_memory = (u8*)malloc(json_size);

  //probe_page_faults(4096);
  while (true)
  {
    /*
    printf("malloc:\n");
    read_file_realloc(json_file_name, json_size);
    printf("\n");

    printf("pre-malloc:\n");
    read_file(json_file_name, json_size, json_memory);
    printf("\n");

    printf("mmap:\n");
    read_file_mmap(json_file_name, json_size);
    printf("\n");

    printf("mmap huge:\n");
    read_file_mmap_huge(json_file_name, json_size);
    printf("\n");

    printf("mmap pre populated:\n");
    read_file_mmap_precommit(json_file_name, json_size);
    printf("\n");
    */

    printf("mmap rounded to page:\n");
    read_file_mmap_rounded_to_page(json_file_name, json_size);
    printf("\n");
  }
  


  TIMED_BLOCK("parse");
  while (cursor < json_size)
    {
      u8 character = *(json_memory+cursor);
      cursor++;
      current_word[current_word_index] = character;
      current_word_index++;

#if 0
      if (str_contains_any(current_word, (u8*)" \n\r\t,{}[]:"))
#endif
#if 1
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
#endif
        {
          current_word_index--;
          current_word[current_word_index] = 0;

#if 0
          if (strncmp(current_word, "null", 4) == 0)
            {
              //
            }
          else if (strncmp(current_word, "true", 4) == 0)
            {
              //
            }
          else if (strncmp(current_word, "false", 5) == 0)
            {
              // 
            }
#endif
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
