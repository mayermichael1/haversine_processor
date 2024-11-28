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

void
init_page_fault_counter();

u64
get_page_fault_count ();

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

///
///   repetition testing
///

struct repetition_test_data
{
  u64 current_time;
  u64 run_to_time;
  f32 seconds_before_quit;

  u64 current_start;
  u64 current_end;
  u64 current_elapsed;

  u64 current_elapsed_page_faults;
  u64 current_start_page_faults;
  u64 current_end_page_faults;

  u64 max_elapsed;
  u64 min_elapsed;

  u64 page_faults_at_max_elapsed;
  u64 page_faults_at_min_elapsed;

  u64 bytes_processed;
};

#define REPETITION_TEST_START(seconds) \
  { \
  repetition_test_data data = {}; \
  data.current_time = get_os_time(); \
  data.seconds_before_quit = seconds; \
  data.run_to_time = data.current_time + (u64)( data.seconds_before_quit * get_os_timer_frequency()); \
  data.min_elapsed = UINT64_MAX; \
  while (data.current_time < data.run_to_time) \
    { \
      data.current_start_page_faults = get_page_fault_count();

#define REPETITION_START_TIMER() data.current_start = get_cpu_time();

#define REPETITION_END_TIMER() data.current_end = get_cpu_time();

#define REPETITION_TEST_END(frequency, bytes) \
  data.current_end_page_faults = get_page_fault_count(); \
  data.current_elapsed_page_faults = data.current_end_page_faults - data.current_start_page_faults; \
  data.current_elapsed = data.current_end - data.current_start; \
  if (data.current_elapsed < data.min_elapsed) \
    { \
      data.min_elapsed = data.current_elapsed; \
      data.page_faults_at_min_elapsed = data.current_elapsed_page_faults; \
      data.run_to_time = data.current_time + (u64)( data.seconds_before_quit * get_os_timer_frequency()); \
    } \
  if (data.current_elapsed > data.max_elapsed) \
    { \
      data.max_elapsed = data.current_elapsed; \
      data.page_faults_at_max_elapsed = data.current_elapsed_page_faults; \
      data.run_to_time = data.current_time + (u64)( data.seconds_before_quit * get_os_timer_frequency()); \
    } \
  data.current_time = get_os_time(); \
  } \
  if (frequency > 0) \
    { \
      f64 min_seconds = ((f64)data.min_elapsed/(f64)frequency); \
      f64 max_seconds = ((f64)data.max_elapsed/(f64)frequency); \
      printf("min: %09.8lf sec\tmax: %09.8f sec\n", min_seconds, max_seconds); \
      if (bytes > 0) \
        { \
          f64 gigabytes = bytes / 1024.f / 2024.f / 2024.f; \
          printf("min test bandwidth: %lf gb/s\tmax test bandwidth: %lf gb/s \n", gigabytes / min_seconds, gigabytes / max_seconds); \
        } \
    } \
  printf("min: %010lu cycles\tmax: %010lu cycles\n", data.min_elapsed, data.max_elapsed); \
  printf("Page Faults(min time): %010lu faults\tPage Faults(max time): %010lu faults\n", data.page_faults_at_min_elapsed, data.page_faults_at_max_elapsed); \
  }

#endif
