#include <stdio.h>
#include "profiler.h"
#include "types.h"
#include <sys/mman.h>

extern "C" void bandwidth_test(u64 read_count, u8* memory);

s32 
main (s32 argc, u8** argv )
{
  u8 *memory;
  u64 GB = 1024 * 1024 * 1024;
  u64 size = GB * 3;

  memory = (u8*)mmap(
    NULL, 
    size, 
    PROT_READ | PROT_WRITE, 
    MAP_PRIVATE | MAP_ANONYMOUS | MAP_POPULATE,
    0, 
    0 
  );

  init_page_fault_counter();
  u64 frequency = estimate_cpu_frequencies();
  f32 test_for_seconds = 5;

  printf("bandwidth_test: \n");
  REPETITION_TEST_START(test_for_seconds);
  REPETITION_START_TIMER();
  bandwidth_test(size, memory);
  REPETITION_END_TIMER();
  REPETITION_TEST_END(frequency);

  print_profiler();
  return 1;
}



