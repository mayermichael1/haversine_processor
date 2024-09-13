#include <stdio.h>
#include <stdlib.h>

#include "types.h"

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

  FILE *fp = fopen(json_file_name, "rb");
  if (fp)
    {
      fseek(fp, 0, SEEK_END);
      json_size = ftell(fp); 
      rewind(fp);
      json_memory = (u8*)malloc(json_size);
      fread(json_memory, sizeof(u8), json_size, fp);
      fclose(fp);
    }

  if (json_size > 10)
    {
      printf("%c \n", json_memory[(json_size-2)]);
    } 
  return 0;
}
