#ifndef METRIC_H
#define METRIC_H

#include <sys/time.h>
#include <x86intrin.h>
#include "types.h"

u64
get_os_timer_frequency ();

u64 
get_os_time ();

u64
get_cpu_time ();

u64 
estimate_cpu_frequencies (u64 milliseconds_to_query = 1000);


struct 
event_data
{
  const char* title;
  u64 start_time;
  u64 end_time;
  u64 elapsed;
};

struct 
timed_block
{
  timed_block(const char* title);
  ~timed_block();
  event_data data;
};

#define TIMED_BLOCK_COUNTED__(number, arg) timed_block profiler_event_##number(arg) 
#define TIMED_BLOCK_COUNTED_(number, arg) TIMED_BLOCK_COUNTED__(number, arg) 
#define TIMED_BLOCK(arg) { TIMED_BLOCK_COUNTED_(__COUNTER__, arg)
#define TIMED_BLOCK_END(arg) }

#define MAX_EVENT_COUNT 1024

struct
profiler_data
{
  event_data events[MAX_EVENT_COUNT];
  u64 event_count;
};

extern profiler_data profiler;

#endif
