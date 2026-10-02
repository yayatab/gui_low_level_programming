//
// Created by Yair Taboch on 27/09/2026.
//

#include <stdlib.h>
#include "thread_pool.h"

#include "SDL3/SDL_cpuinfo.h"

static int worker_thread_fn(void* data) {
  ThreadPool* pool = data;
  while (!pool->shutdown) {
    SDL_LockMutex(pool->lock);
    while (!pool->shutdown && pool->current_index >= pool->end_index) {
      SDL_WaitCondition(pool->work_available, pool->lock);
    }
    if (pool->shutdown) {
      SDL_UnlockMutex(pool->lock);
      break;
    }
    if (pool->current_index >= pool->end_index || pool->tasks_remaining == 0) {
      SDL_UnlockMutex(pool->lock);
      continue;
    }

    int index = pool->current_index++;
    ParallelForFunc func = pool->task_func;
    SDL_UnlockMutex(pool->lock);
    func(index, pool->user_data);
    SDL_LockMutex(pool->lock);
    pool->tasks_remaining--;
    if (pool->tasks_remaining == 0) {
      SDL_SignalCondition(pool->work_done);
    }
    SDL_UnlockMutex(pool->lock);
  }
  return 0;

}


int thread_pool_init(ThreadPool* pool, size_t thread_count) {

  pool->lock = SDL_CreateMutex();
  pool->work_available = SDL_CreateCondition();
  pool->work_done = SDL_CreateCondition();
  pool->shutdown = false;
  pool->thread_count = thread_count > 0 ? thread_count: SDL_GetNumLogicalCPUCores();
  pool->threads = SDL_malloc(pool->thread_count * sizeof(SDL_Thread*));
  pool->current_index = 0;
  pool->tasks_remaining=0;
  pool->end_index =0;
  char name[256];
  for (int i = 0; i < pool->thread_count; i++) {
    sprintf(name, "thread_pool_%lu", pool->thread_count);
    pool->threads[i] = SDL_CreateThread(worker_thread_fn, name, pool);
  }
  return 0;
}

void thread_pool_parallel_for(ThreadPool* pool, int start, int end, ParallelForFunc func, void* user_data) {
  if (start >= end || func == NULL) {
    return;
  }
  SDL_LockMutex(pool->lock);
  pool->user_data = user_data;
  pool->task_func = func;
  pool->current_index = start;
  pool->end_index = end;
  pool->tasks_remaining = end - start;
  SDL_BroadcastCondition(pool->work_available);
  while (pool->tasks_remaining > 0) {
    SDL_WaitCondition(pool->work_done, pool->lock);
  }
  SDL_UnlockMutex(pool->lock);
}

void thread_pool_destroy(ThreadPool* pool){
  SDL_LockMutex(pool->lock);
  pool->shutdown = true;
  SDL_BroadcastCondition(pool->work_available);
  SDL_UnlockMutex(pool->lock);
  for (int i = 0; i < pool->thread_count; i++) {
    SDL_WaitThread(pool->threads[i], NULL);
  }
  SDL_free(pool->threads);
  SDL_DestroyCondition(pool->work_available);
  SDL_DestroyCondition(pool->work_done);
  SDL_DestroyMutex(pool->lock);
}
