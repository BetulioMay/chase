#ifndef _THREAD_H
#define _THREAD_H

#include "directory.h"
#include <dirent.h>
#include <pthread.h>

#define MAX_NUM_THREADS 10

typedef struct _thread_args
{
  unsigned int tid;
  DIR* dirp;
  char* initial_dir;
  unsigned int num_tasks;
} THREAD_ARGS;

void* thread_func(void* args_void);

unsigned int create_threads(pthread_t* threads, THREAD_ARGS* common_args);

void assign_workload(DIR **dirs, int num_workers, const char *dirpath, unsigned int dirs_thread);

#endif
