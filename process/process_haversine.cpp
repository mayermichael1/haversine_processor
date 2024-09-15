#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "types.h"

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

s32
main (s32 argc, u8 **argv)
{
  u8 *json_file_name = NULL;
  s32 json_size = 0;
  u8 *json_memory = NULL;

  if (argc >= 2)
    {
      json_file_name = argv[1];
    }
  
  json_memory = read_file(json_file_name, &json_size);

  s32 cursor = 0;
  s32 current_word_index = 0;
  u8 current_word[256] = "";

  while (cursor < json_size)
    {
      u8 character = *(json_memory+cursor);
      cursor++;
      current_word[current_word_index] = character;
      current_word_index++;

      switch (character)
        {
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

              if (strncmp(current_word, "null", 4) == 0)
                {
                  printf ("null read\n");
                }
              else if (strncmp(current_word, "true", 4) == 0)
                {
                  printf ("true read\n");
                }
              else if (strncmp(current_word, "false", 5) == 0)
                {
                  printf ("false read\n");
                }
              else if (current_word[0] == '"' &&
                       current_word[current_word_index - 1] == '"')
                {
                  current_word[current_word_index - 1] = 0;
                  printf("identifier: %s \n", &current_word[1]);
                }
              else 
                {
                  u8* endptr;
                  f64 value = strtof(current_word, &endptr); 
                  if (endptr != NULL || endptr == current_word)
                    {
                      printf("value: %f \n", value);
                    }
                }

              current_word_index = 0;
              break;
            }
        }
    }
  return 0;
}
