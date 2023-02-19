// Author: Alexey Kasyanchuk <degitx@gmail.com>

#pragma once

#include <iostream>
#include <string.h>
#include <sys/time.h>

#define DX_COLORFUL 1

#ifdef CPK
extern int dx_print_level;
#else
static int dx_print_level = 2;
#endif

static void DX_SET_DEBUG_LEVEL(int level = 2)
{
  dx_print_level = level;
}

#ifdef DX_COLORFUL

inline int DX_STRING_HASH(const char* str)
{
  int hash = 0;
  for (; *str; ++str)
  {
    hash  = ((hash << 5) - hash) + *str;
  }
  return hash;
}

#define DX_PRINTF(out, tag, level, msg, ...) {\
  timeval curTime;\
  time_t rawtime;\
  char timebuffer[28];\
  if (level <= dx_print_level)\
  {\
  gettimeofday(&curTime, NULL);\
  rawtime = curTime.tv_sec;\
  strftime(timebuffer, 28, "%H:%M:%S", localtime(&rawtime));\
  \
  char color[32] = {0}; \
  short color_pick = DX_STRING_HASH(tag) % (232 - 16 + 1) + 16; \
  sprintf(color, "\x1b[38;5;%dm", color_pick); \
  char color_clear[] = "\x1b[0m"; \
  \
  char text_color[16] = { 0 };\
  char text_clear[16] = { 0 };\
  if (level == 0) {\
    sprintf(text_color, "\x1b[38;5;%dm", 1);\
    sprintf(text_clear, "\x1b[0m");\
  } else if (level == 1) {\
    sprintf(text_color, "\x1b[38;5;%dm", 3);\
    sprintf(text_clear, "\x1b[0m");\
  }\
  fprintf(out, "[%s:%03ld] %s[%s]%s %s" msg "%s\n", timebuffer, curTime.tv_usec / 1000, color, tag, color_clear, text_color, ##__VA_ARGS__, text_clear);\
  }\
}

#define DX_ERROR(tag, ...) DX_PRINTF(stderr, tag, 0, ##__VA_ARGS__)
#define DX_WARN(tag, ...) DX_PRINTF(stdout, tag, 1, ##__VA_ARGS__)
#define DX_INFO(tag, ...) DX_PRINTF(stdout, tag, 2, ##__VA_ARGS__)
#define DX_DEBUG(tag, ...) DX_PRINTF(stdout, tag, 3, ##__VA_ARGS__)

#else // DX_COLORFUL
#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#define DX_PRINTF(out, tag, color, type, msg, ...) {\
  timeval curTime;\
  time_t rawtime;\
  char timebuffer[28];\
  gettimeofday(&curTime, NULL);\
  rawtime = curTime.tv_sec;\
  strftime(timebuffer, 28, "%H:%M:%S", localtime(&rawtime));\
\
  fprintf(out, "[%s:%03ld] [%s] %s:%d: %s: " msg "\n", timebuffer, curTime.tv_usec / 1000, "\x1B[" color "m" type "\033[0m", __FILENAME__, __LINE__, __FUNCTION__, ##__VA_ARGS__);\
}

#define DX_ERROR(tag, ...) DX_PRINTF(stderr, tag, "31", "E", ##__VA_ARGS__)
#define DX_WARN(tag, ...) DX_PRINTF(stdout, tag, "33", "W", ##__VA_ARGS__)
#define DX_INFO(tag, ...) DX_PRINTF(stdout, tag, "32", "I", ##__VA_ARGS__)
#define DX_DEBUG(tag, ...) DX_PRINTF(stdout, tag, "36", "D", ##__VA_ARGS__)
#endif // DX_COLORFUL

static uint64_t DXGetSystemNanoTime()
{
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    uint64_t currentSystemTime = now.tv_sec * 1000000000LL + now.tv_nsec;
 
    return currentSystemTime;
}
 
// содержит timestamp!
static uint64_t DXGetMicroTime()
{
	struct timeval currentTime;
    gettimeofday(&currentTime, NULL);
    return currentTime.tv_sec * 1000000LL + currentTime.tv_usec;
}

// содержит timestamp!
static uint64_t DXGetMiliTime()
{
	return DXGetMicroTime() / 1000;
}

#define DX_START_NANO_TIMER(dx_nano_timer_tag) uint64_t start_##dx_nano_timer_tag = DXGetSystemNanoTime();
#define DX_STOP_NANO_TIMER(dx_nano_timer_tag) uint64_t diff_##dx_nano_timer_tag = DXGetSystemNanoTime() - start_##dx_nano_timer_tag;
#define DX_STOP_NANO_TIMER_PRINT(dx_nano_timer_tag) DX_INFO("%ld ns ["#dx_nano_timer_tag"]", DXGetSystemNanoTime() - start_##dx_nano_timer_tag);
#define DX_GET_NANO_TIMER(dx_nano_timer_tag) diff_##dx_nano_timer_tag

#define DX_START_MICRO_TIMER(dx_micro_timer_tag) uint64_t start_##dx_micro_timer_tag = DXGetMicroTime();
#define DX_STOP_MICRO_TIMER(dx_micro_timer_tag) uint64_t diff_##dx_micro_timer_tag = DXGetMicroTime() - start_##dx_micro_timer_tag;
#define DX_STOP_MICRO_TIMER_PRINT(dx_micro_timer_tag) DX_INFO("%ld qs ["#dx_micro_timer_tag"]", DXGetMicroTime() - start_##dx_micro_timer_tag);
#define DX_GET_MICRO_TIMER(dx_micro_timer_tag) diff_##dx_micro_timer_tag

#define DX_START_MILI_TIMER(dx_mili_timer_tag) uint64_t start_##dx_mili_timer_tag = DXGetMiliTime();
#define DX_STOP_MILI_TIMER(dx_mili_timer_tag) uint64_t diff_##dx_mili_timer_tag = DXGetMiliTime() - start_##dx_mili_timer_tag;
#define DX_STOP_MILI_TIMER_PRINT(dx_mili_timer_tag) DX_INFO("%ld ms ["#dx_mili_timer_tag"]", DXGetMiliTime() - start_##dx_mili_timer_tag);
#define DX_GET_MILI_TIMER(dx_mili_timer_tag) diff_##dx_mili_timer_tag

#define DX_START_HU_TIMER(dx_nano_timer_tag) uint64_t start_##dx_nano_timer_tag = DXGetSystemNanoTime();
#define DX_STOP_HU_TIMER_PRINT(dx_nano_timer_tag) \
  uint64_t diff_##dx_nano_timer_tag = DXGetSystemNanoTime() - start_##dx_nano_timer_tag; \
  if (diff_##dx_nano_timer_tag >= 1000 * 1000) { \
    DX_INFO("%f ms ["#dx_nano_timer_tag"]", (double)diff_##dx_nano_timer_tag / 1000000); \
  } else if(diff_##dx_nano_timer_tag >= 1000) { \
    DX_INFO("%f qs ["#dx_nano_timer_tag"]", (double)diff_##dx_nano_timer_tag / 1000); \
  } else { \
    DX_INFO("%f ns ["#dx_nano_timer_tag"]", diff_##dx_nano_timer_tag); \
  }


#define DX_BENCHMARK_FIRST(dx_nano_timer_tag) uint64_t start_benchmark1_##dx_nano_timer_tag = DXGetSystemNanoTime();
#define DX_BENCHMARK_SECOND(dx_nano_timer_tag) \
  uint64_t diff_benchmark1_##dx_nano_timer_tag = DXGetSystemNanoTime() - start_benchmark1_##dx_nano_timer_tag; \
  uint64_t start_benchmark2_##dx_nano_timer_tag = DXGetSystemNanoTime();
#define DX_BENCHMARK_END(dx_nano_timer_tag) \
  uint64_t diff_benchmark2_##dx_nano_timer_tag = DXGetSystemNanoTime() - start_benchmark2_##dx_nano_timer_tag; \
  int64_t diff_benchmarks_##dx_nano_timer_tag = diff_benchmark1_##dx_nano_timer_tag - diff_benchmark2_##dx_nano_timer_tag; \
  if (diff_benchmarks_##dx_nano_timer_tag >= 1000 * 1000) { \
    DX_INFO("%s%f ms ["#dx_nano_timer_tag"]", diff_benchmarks_##dx_nano_timer_tag > 0 ? "-" : "+", (double)diff_benchmarks_##dx_nano_timer_tag / 1000000); \
  } else if(diff_benchmarks_##dx_nano_timer_tag >= 1000) { \
    DX_INFO("%s%f qs ["#dx_nano_timer_tag"]", diff_benchmarks_##dx_nano_timer_tag > 0 ? "-" : "+", (double)diff_benchmarks_##dx_nano_timer_tag / 1000); \
  } else { \
    DX_INFO("%s%f ns ["#dx_nano_timer_tag"]", diff_benchmarks_##dx_nano_timer_tag > 0 ? "-" : "+", diff_benchmarks_##dx_nano_timer_tag); \
  }

