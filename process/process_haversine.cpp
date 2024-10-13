#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "types.h"
#include "calc.h"
#include "profiler.h"

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
read_file (u8 *file_name, s32 bytes_to_read, u8* memory)
{
  repetiton_test_data reptest_data = {};
  REPETITION_TEST_START(reptest_data, 10.f);

  FILE *fp = fopen(file_name, "rb");

  if (fp)
    {
      REPETITION_START_TIMER(reptest_data);
      fread(memory, sizeof(u8), bytes_to_read, fp);
      REPETITION_END_TIMER(reptest_data);

      fclose(fp);
    }

  REPETITION_TEST_END(reptest_data);
  printf("min: %lu\tmax: %lu\n", reptest_data.min_elapsed, reptest_data.max_elapsed);

  return memory;
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

  //estimate_cpu_frequencies();

  if (argc >= 2)
    {
      json_file_name = argv[1];
    }
  

  json_size = get_file_size(json_file_name);
  json_memory = (u8*)malloc(json_size);
  read_file(json_file_name, json_size, json_memory);

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
