#ifndef _THREAD_H
#define _THREAD_H

#include "directory.h"
#include <dirent.h>
#include <pthread.h>

#define MAX_NUM_THREADS 5

typedef struct _thread_args
{
  // Thread id
  unsigned int tid;

  // Current dir pointer
  DIR* dirp;

  // Parent dir of searching tree
  char* initial_dir;

  // Thread tasks slice
  unsigned int num_tasks;

  // searching options
  CHASE_OPTS* ch_opts;

} THREAD_ARGS;

void* thread_func(void* args_void);

unsigned int create_threads(pthread_t* threads, THREAD_ARGS* common_args);

void assign_workload(DIR **dirs, int num_workers, const char *dirpath, unsigned int dirs_thread);

#endif
