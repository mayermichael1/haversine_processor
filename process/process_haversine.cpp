#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "types.h"
#include "calc.h"
#include "profiler.h"

static u8*
read_file (u8 *file_name, s32 *size)
{
  FILE *fp = fopen(file_name, "rb");
  u8 *memory = NULL;

  if (fp)
    {
      fseek(fp, 0, SEEK_END);
      *size = ftell(fp); 
      rewind(fp);
      memory = (u8*)malloc(*size);
      fread(memory, sizeof(u8), *size, fp);
      fclose(fp);
    }

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
  u64 timer_start = get_cpu_time();
  u64 timer_elapsed = 0;
  u64 read_elapsed = 0;
  u64 parser_elapsed = 0;
  u64 calc_elapsed = 0;

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
  
  u64 read_start = get_cpu_time();
  json_memory = read_file(json_file_name, &json_size);
  u64 read_end = get_cpu_time();
  read_elapsed = read_end - read_start;

  u64 parse_start = get_cpu_time();
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
                      u64 calc_start = get_cpu_time();
                      coords[1].latitude = value;
                      haversine_sum += reference_haversine(
                        coords[0],
                        coords[1],
                        EARTH_RADIUS
                      );
                      haversine_calc_ammount++;
                      coords[0] = {0.0, 0.0};
                      coords[1] = {0.0, 0.0};
                      u64 calc_end = get_cpu_time();
                      calc_elapsed += (calc_end - calc_start);
                    }
                }
            }

          current_word_index = 0;
          break;
        }
    }
  u64 parse_end = get_cpu_time(); 
  parser_elapsed = (parse_end - parse_start) - calc_elapsed;

  printf("Processed average: %f\n", (haversine_sum/(f64)haversine_calc_ammount));

  u64 timer_end = get_cpu_time();
  timer_elapsed = timer_end - timer_start;

  printf("Profiling: \n");
  printf("read:    %15lu \t %.2f\n", read_elapsed, (f64)read_elapsed / (f64)timer_elapsed);
  printf("parse:   %15lu \t %.2f \n", parser_elapsed, (f64)parser_elapsed / (f64)timer_elapsed);
  printf("calc:    %15lu \t %.2f\n", calc_elapsed, (f64)calc_elapsed / (f64)timer_elapsed);
  printf("program: %15lu \t 1.00\n", timer_elapsed);
  return 0;
}
