#include "profiler.h"
#include <stdio.h>

u64
get_os_timer_frequency ()
{
  return 1000000;
}

u64 
get_os_time ()
{
  struct timeval time = {};
  gettimeofday(&time, 0);
  u64 time_microseconds = time.tv_sec * get_os_timer_frequency() + time.tv_usec;
  return time_microseconds;
}

u64
get_cpu_time ()
{
  u64 time = __rdtsc();
  return time;
}

u64 
estimate_cpu_frequencies (u64 milliseconds_to_query)
{
  f64 seconds_to_query = milliseconds_to_query / 1000.0;
  u64 os_frequency = get_os_timer_frequency();
  u64 microseconds_to_query = seconds_to_query * os_frequency; 

  u64 os_start = get_os_time();
  u64 os_end = 0;
  u64 os_elapsed = 0;

  u64 cpu_start = get_cpu_time();
  u64 cpu_end = 0;
  u64 cpu_elapsed = 0;

  while (os_elapsed < microseconds_to_query)
    {
      os_end = get_os_time();
      os_elapsed = os_end - os_start;
    }
  os_end = get_os_time();
  os_elapsed = os_end - os_start;

  cpu_end = get_cpu_time();
  cpu_elapsed = cpu_end - cpu_start;

  f64 seconds_run = (f64)microseconds_to_query / (f64)os_frequency;
  
  u64 estimated_cpu_frequency = cpu_elapsed / seconds_run;
  return estimated_cpu_frequency;
}
