#include <stdio.h>
#include <stdlib.h>

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

  for (s32 cursor = 0; cursor < json_size; cursor++)
    {
      u8 character = *(json_memory+cursor);

      switch (character)
        {
          case ' ':
          case '\n':
          case '\r':
          case '\t':
            {
              continue;
              break;
            }
          case '{':
            {
              printf("Begin Object\n");
              break;
            }
          case '}':
            {
              printf("End Object\n");
              break;
            }
          case '[':
            {
              printf("Begin Array\n");
              break;
            }
          case ']':
            {
              printf("End Array\n");
              break;
            }
          case '"':
            {
              printf("String follows\n");
              break;
            }
          case 'n':
            {
              printf("null follows\n");
              break;
            }
          case 't':
            {
              printf("true follows\n");
              break;
            }
          case 'f':
            {
              printf("false follows\n");
              break;
            }
          case '0':
          case '1':
          case '2':
          case '3':
          case '4':
          case '5':
          case '6':
          case '7':
          case '8':
          case '9':
            {
              printf("number follow\n");
              break;
            }
          default:
            continue;
        }

      // possible json beginnings
      // {
      // [
    }
  return 0;
}
