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

s16
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
        if (current_key >= MAX_EVENT_COUNT)
          {
            current_key = 0;
          }
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
        if (current_key >= MAX_EVENT_COUNT)
          {
            current_key = 0;
          }
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

timed_block::timed_block(char *title)
{
  data.title = title;
  data.start_time = get_cpu_time();
}

timed_block::~timed_block()
{
  data.end_time = get_cpu_time();
  data.elapsed = data.end_time - data.start_time;

  profiler_insert_event(data);
}
