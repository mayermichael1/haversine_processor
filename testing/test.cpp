#include <stdio.h>
#include "profiler.h"
#include "types.h"
#include <sys/mman.h>

extern "C" void nt_test(u64 buffer_size, u8 *buffer, u64 data_size, u8 *data);
extern "C" void test(u64 buffer_size, u8 *buffer, u64 data_size, u8 *data);

#define to_u32(value) *(u32*)(&value)

s32 
main (s32 argc, u8** argv )
{
    u64 KB = 1024;
    u64 MB = KB * 1024;
    u64 GB = MB * 1024;

    u8 *buffer;
    u64 buffer_size = GB * 3;
    u8 *data;
    u64 data_size = 32 * KB;

    buffer = (u8*)mmap(
        NULL, 
        buffer_size, 
        PROT_READ | PROT_WRITE, 
        MAP_PRIVATE | MAP_ANONYMOUS | MAP_POPULATE,
        0, 
        0 
    );

    data = (u8*)mmap(
        NULL, 
        data_size, 
        PROT_READ | PROT_WRITE, 
        MAP_PRIVATE | MAP_ANONYMOUS | MAP_POPULATE,
        0, 
        0 
    );

    for (u64 i = 0; i < data_size; i+=8)
    {
        to_u32(data[i]) = i;
    }

    init_page_fault_counter();
    u64 frequency = estimate_cpu_frequencies();
    f64 seconds = 5.0;

    TIMED_BANDWITH("nt_test", buffer_size);
    nt_test(buffer_size, buffer, data_size, data);
    TIMED_BANDWITH_END("nt_test");

    TIMED_BANDWITH("test", buffer_size);
    test(buffer_size, buffer, data_size, data);
    TIMED_BANDWITH_END("test");

    /*
    for (u64 i = 0; i < buffer_size; i+=data_size)
        {
            printf("\n\n\n");
            for (u64 d = 0; d < data_size; d+=64)
            {
                for (u64 c = 0; c < 64; c+=8)
                {
                    printf("%08i ", to_u32(buffer[i + d + c ]));
                }
                printf("\n");
            }
            printf("\n\n\n");
        }
    */


    print_profiler();
    return 1;
}

