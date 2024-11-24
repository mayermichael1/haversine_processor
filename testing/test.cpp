#include <stdio.h>
#include "profiler.h"
#include "types.h"

extern "C" void read_x_1(s32 count, u8* character);
extern "C" void read_x_2(s32 count, u8* character);
extern "C" void read_x_3(s32 count, u8* character);
extern "C" void read_x_4(s32 count, u8* character);
extern "C" void read_x_5(s32 count, u8* character);
extern "C" void read_x_6(s32 count, u8* character);

s32 
main (s32 argc, u8** argv )
{
  u8 character = 'u';
  u64 GB = 1024 * 1024 * 1024;
  u64 size = GB * 3;

  TIMED_BANDWITH("read_x_1", size);
  read_x_1(size, &character);
  TIMED_BANDWITH_END("read_x_1");

  TIMED_BANDWITH("read_x_2", size);
  read_x_2(size, &character);
  TIMED_BANDWITH_END("read_x_2");

  TIMED_BANDWITH("read_x_3", size);
  read_x_3(size, &character);
  TIMED_BANDWITH_END("read_x_3");

  TIMED_BANDWITH("read_x_4", size);
  read_x_4(size, &character);
  TIMED_BANDWITH_END("read_x_4");

  TIMED_BANDWITH("read_x_5", size);
  read_x_5(size, &character);
  TIMED_BANDWITH_END("read_x_5");

  TIMED_BANDWITH("read_x_6", size);
  read_x_6(size, &character);
  TIMED_BANDWITH_END("read_x_6");
j 
  print_profiler();
  return 1;
}



