#include "profiler.h"
#include <stdio.h>
#include <string.h>

profiler_data profiler = {0};

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

inline b8
events_spot_filled (u16 index)
{
  return profiler.events[index].title != 0 &&
         strlen(profiler.events[index].title) > 0;
}

static u16
calc_event_hash (char *title)
{
  u16 hashkey = 0;
  if (title != 0)
    {
      for (u8 *character = title; *character != 0; character++)
        {
          hashkey += *character;
        }
    }
  hashkey = hashkey % MAX_EVENT_COUNT;
  return hashkey;
}

static inline void
wrap_profiler_event_counter(u16 *counter)
{
  if (*counter >= MAX_EVENT_COUNT)
    {
      *counter = 0;
    }
}

static s16
find_key (char *title)
{
  s16 found_key = -1;
  u16 current_key = calc_event_hash(title);
  u16 hash_key = current_key;

  do
  {
    if (events_spot_filled(current_key) &&
        strcmp(profiler.events[current_key].title, title) == 0)
      {
        found_key = current_key; 
        break;
      }
    else
      {
        current_key++;
        wrap_profiler_event_counter(&current_key);
      }
  } 
  while(hash_key != current_key);
  
  return found_key;
}

static s16
find_next_free (char *title)
{  
  s16 found_key = -1;
  u16 current_key = calc_event_hash(title);
  u16 hash_key = current_key;

  do
  {
    if (!events_spot_filled(current_key))
      {
        found_key = current_key; 
        break;
      }
    else if (strcmp(profiler.events[current_key].title, title) == 0)
      {
        break;
      }
    else
      {
        current_key++;
        wrap_profiler_event_counter(&current_key);
      }
  } 
  while(hash_key != current_key);
  
  return found_key;
}

void
profiler_insert_event (event_data data)
{
  s16 exists_key = find_key(data.title);
  if (exists_key >= 0)
    {
      profiler.events[exists_key].elapsed += data.elapsed;
      profiler.events[exists_key].bytes_processed += data.bytes_processed;
    }
  else
    {
      s16 key = find_next_free(data.title);
      if (key >= 0)
        {
          profiler.events[key] = data;
        }
    }
}

event_data 
profiler_get_event (char *title)
{
  event_data data = {};
  s16 key = find_key(title);
  if (key >= 0)
    {
      data = profiler.events[key]; 
    } 
  return data;
}

b8 
profiler_iterate (event_data *event)
{
  b8 next_found = false;
  s16 key = 0;

  if (event->title != 0)
    {
      key = find_key(event->title);
      if (key >= 0)
        {
          key++;
        }
    }

  if (key >= 0)
    {
      for (int i = key; i < MAX_EVENT_COUNT; i++)
        {
          if (events_spot_filled(i))
            {
              *event = profiler.events[i];
              next_found = true;
              break;
            }
        }
    } 
  return next_found;
}

timed_block::timed_block(char *title, u64 bytes_processed)
{
  data = {};
  data.title = title;
  data.start_time = get_cpu_time();
  data.bytes_processed += bytes_processed;
}

timed_block::~timed_block()
{
  data.end_time = get_cpu_time();
  data.elapsed = data.end_time - data.start_time;

  profiler_insert_event(data);
}

void 
print_profiler()
{
#ifdef DEBUG
  u64 cpu_frequency = estimate_cpu_frequencies();

  event_data data = {};

  while(profiler_iterate(&data))
    {
      f64 elapsed_seconds = (f64)data.elapsed / (f64)cpu_frequency;
      printf("%s:\t%5.5f \t%15lu", data.title, elapsed_seconds, data.elapsed);

      if (data.bytes_processed > 0) 
        {
          f64 mb_processed = data.bytes_processed / 1024.f / 1024.f;
          f64 gb_processed_per_second = (mb_processed / 1024.f) / elapsed_seconds;
          printf("\tmb: %9.5f\tgb/s: %9.5f", mb_processed, gb_processed_per_second);
        }

      printf("\n");
    } 
#endif
}

#include <linux/perf_event.h>
#include <linux/hw_breakpoint.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <sys/ioctl.h>

static s32 page_fault_file_descriptor = 0;

static s64 
perf_event_open(struct perf_event_attr *hw_event, pid_t pid, s32 cpu,
                s32 group_fd, u64 flags)
{
  u32 return_value;
  return_value = syscall(SYS_perf_event_open, hw_event, pid, cpu, group_fd, flags);
  return return_value;
}

void
init_page_fault_counter()
{
  struct perf_event_attr data = {};

  data.type = PERF_TYPE_SOFTWARE;
  data.size = sizeof(perf_event_attr);
  data.config = PERF_COUNT_SW_PAGE_FAULTS_MIN;
  data.exclude_hv = 1;
  //NOTE: with this parameter off (=0 default) sudo is required to see page faults
  //NOTE: with this paramter on  (=1) only 1 page fault is reported instead of 
  //      the real number
  //data.exclude_kernel = 1;

  page_fault_file_descriptor = perf_event_open(&data, 0, -1, -1, 0);
}

u64
get_page_fault_count ()
{ 
  u64 count = 0;
  if (page_fault_file_descriptor != -1)
    {
      ioctl(page_fault_file_descriptor, PERF_EVENT_IOC_REFRESH);
      read(page_fault_file_descriptor, &count, sizeof(count));
    }

  return count;
}
