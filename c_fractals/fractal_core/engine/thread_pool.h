#pragma once
#include "SDL3/SDL_cpuinfo.h"
#include "SDL3/SDL_mutex.h"
#include "SDL3/SDL_thread.h"

#define CALC_MIN_THREAD_COUNT(a,b) (((a) < (b)) ? (a) : (b))
#define MAX_THREAD_COUNT(a) CALC_MIN_THREAD_COUNT(a, SDL_GetNumLogicalCPUCores())


typedef void (*ParallelForFunc)(int index, void *user_data);

typedef struct {
  // Threads
  SDL_Thread **threads;
  size_t thread_count;

  //Synchronization
  SDL_Mutex *lock;
  SDL_Condition *work_available; // wakes up threads
  SDL_Condition *work_done; // signals when work is done

  // Task State
  ParallelForFunc task_func;
  void *user_data;
  int current_index;
  int end_index;
  int tasks_remaining;
  bool shutdown;
} ThreadPool;

int thread_pool_init(ThreadPool *pool, size_t thread_count);

void thread_pool_parallel_for(ThreadPool *pool, int start, int end, ParallelForFunc func, void *user_data);

void thread_pool_destroy(ThreadPool *pool);