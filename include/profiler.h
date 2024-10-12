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

#define MAX_EVENT_COUNT 1024

struct 
event_data
{
  u8* title;
  u64 start_time;
  u64 end_time;
  u64 elapsed;
  u64 bytes_processed;
};

struct 
timed_block
{
  timed_block(u8* title, u64 bytes_processed);
  ~timed_block();
  event_data data;
};

///
///   PROFILER
///

//NOTE: this profiler accumulates events of the same name.
//      start and end time are of the last accumulated event.
//      elapsed time of the event is summed.


struct
profiler_data
{
  event_data events[MAX_EVENT_COUNT];
};

extern profiler_data profiler;

void
profiler_insert_event (event_data data);

event_data 
profiler_get_event (char *title);

b8 
profiler_iterate (event_data *event);

void
print_profiler();

#ifdef DEBUG

#define TIMED_BANDWITH_COUNTED_(number, title, bytes) timed_block profiler_event_##number((char*)title, bytes) 
#define TIMED_BANDWITH_COUNTED(number, title, bytes) TIMED_BANDWITH_COUNTED_(number, title, bytes) 

#define TIMED_BANDWITH(title, bytes) { TIMED_BANDWITH_COUNTED(__COUNTER__, title, bytes)
#define TIMED_BANDWITH_END(title) }

#define TIMED_BLOCK(arg) { TIMED_BANDWITH_COUNTED(__COUNTER__, arg, 0)
#define TIMED_BLOCK_END(arg) }

//#define TIMED_FUNCTION_(name) TIMED_BLOCK(name);
#define TIMED_FUNCTION() TIMED_BANDWITH_COUNTED(__COUNTER__,__FUNCTION__, 0);

#else //DEBUG

#define TIMED_BLOCK(arg)
#define TIMED_BLOCK_END(arg)
#define TIMED_FUNCTION()
#define TIMED_BANDWITH(title, bytes)
#define TIMED_BANDWITH_END(title)

#endif //DEBUG

#endif
