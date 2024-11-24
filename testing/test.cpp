#include <stdio.h>
#include "profiler.h"
#include "types.h"

extern "C" void read_4x2(s32 count, u8* memory);
extern "C" void read_8x2(s32 count, u8* memory);
extern "C" void read_16x2(s32 count, u8* memory);
extern "C" void read_32x2(s32 count, u8* memory);

s32 
main (s32 argc, u8** argv )
{
  u8 memory[64];
  for (s32 i = 0; i < 64; ++i)
    {
      memory[i] = i % 10;
    }
  u64 GB = 1024 * 1024 * 1024;
  u64 size = GB * 3;

  init_page_fault_counter();
  u64 frequency = estimate_cpu_frequencies();
  f32 test_for_seconds = 5;

  printf("read_4x2");
  REPETITION_TEST_START(test_for_seconds);
  REPETITION_START_TIMER();
  read_4x2(size, memory);
  REPETITION_END_TIMER();
  REPETITION_TEST_END(frequency);

  printf("read_8x2");
  REPETITION_TEST_START(test_for_seconds);
  REPETITION_START_TIMER();
  read_8x2(size, memory);
  REPETITION_END_TIMER();
  REPETITION_TEST_END(frequency);

  printf("read_16x2");
  REPETITION_TEST_START(test_for_seconds);
  REPETITION_START_TIMER();
  read_16x2(size, memory);
  REPETITION_END_TIMER();
  REPETITION_TEST_END(frequency);

  printf("read_32x2");
  REPETITION_TEST_START(test_for_seconds);
  REPETITION_START_TIMER();
  read_32x2(size, memory);
  REPETITION_END_TIMER();
  REPETITION_TEST_END(frequency);

  print_profiler();
  return 1;
}



