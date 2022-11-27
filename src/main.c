#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/limits.h>

#include <pthread.h>
#include <math.h>

#define MAX_NUM_THREADS 10
#define DIRS_PER_THREAD 5

#include "directory.h"

typedef struct _thread_args
{
  unsigned int tid;
  DIR* dirp;
  char* initial_dir;
} THREAD_ARGS;

static pthread_mutex_t mutex;

static void parse_dir_arg(char* dir_arg)
{
  // Get rid of trailing '/'
  unsigned long size = strlen(dir_arg) + 1;

  // remove trailing '/' if not root directory
  if (dir_arg[size-2] == '/' && strlen(dir_arg) > 1)
  {
    dir_arg[size-2] = '\0';
  }
}

void* thread_func(void* args_void)
{
  THREAD_ARGS* args = args_void;

  pthread_mutex_lock(&mutex);
  fprintf(stdout, "Hey! I'm thread %u\n", args->tid);
  pthread_mutex_unlock(&mutex);

  free(args);

  pthread_exit(NULL);
  return NULL;
}

unsigned int create_threads(pthread_t* threads, THREAD_ARGS* common_args)
{
  const unsigned int NUM_FILES = get_num_files(common_args->dirp);
  printf("Number of files: %u\n", NUM_FILES);
  unsigned int num_threads = trunc((NUM_FILES*1.0) / (DIRS_PER_THREAD*1.0));

  if (num_threads > 10)
    num_threads = 10;

  printf("Number of threads to create: %u\n", num_threads);

  for (unsigned int i = 0; i < num_threads && i < MAX_NUM_THREADS; ++i)
  {
    THREAD_ARGS* thread_args = (THREAD_ARGS*)malloc(sizeof(*common_args));
    thread_args->tid = i;

    int t_error = pthread_create(&threads[i], NULL, thread_func, (void*)thread_args);

    if (t_error)
      fprintf(stderr, "ERROR on pthread_create\n");
  }
  return num_threads;
}

int main(int argc, char** argv)
{
  if (argc < 2)
  {
    fprintf(stderr, "ERROR: bad arguments\n");
    exit(EXIT_FAILURE);
  }

  // Parse initial directory
  parse_dir_arg(argv[1]);
 
  // Open directory
  DIR* dirp = open_dir(argv[1]);

  // Init threads
  THREAD_ARGS args = {
    .tid = -1,    // -1 === no id assigned
    .dirp = dirp,
    .initial_dir = argv[1]
  };

  pthread_t threads[MAX_NUM_THREADS];
  int num_threads = create_threads(threads, &args);

  // Start chasing
  //traverse_dir(dirp, argv[1]);

  for (int i = 0; i < num_threads; ++i)
  {
    void* status;
    int t_error = pthread_join(threads[i], &status);
    if (t_error)
      fprintf(stderr, "ERROR on pthread_join %d\n", t_error);
  }

  // Close directory
  close_dir(dirp);

  return 0;
}
