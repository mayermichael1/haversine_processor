#include <stdio.h>
#include "profiler.h"
#include "types.h"
#include <sys/mman.h>

extern "C" void cache_test(u64 read_bytes, u8 *memory, u64 stride, u64 runfor);

s32 
main (s32 argc, u8** argv )
{
    u8 *memory;
    u64 KB = 1024;
    u64 MB = KB * 1024;
    u64 GB = MB * 1024;
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
    f64 seconds = 5.0;

    for (u32 stride = 1; stride <= 16 * KB; stride*=2)
    {
        printf("cache test with %i\n", stride);
        REPETITION_TEST_START(seconds);
        REPETITION_START_TIMER();
        cache_test(16 * KB, memory, stride, size / (16 * KB));
        REPETITION_END_TIMER();
        REPETITION_TEST_END(frequency, size);
        printf("\n");
    }

    //print_profiler();
    return 1;
}

