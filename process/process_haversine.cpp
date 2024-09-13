#include <stdio.h>
#include <stdlib.h>

#include "../include/types.h"

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

  if (json_size > 10)
    {
      printf("%c \n", json_memory[(json_size-2)]);
    } 
  return 0;
}
