#define _GNU_SOURCE
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/limits.h>

#include <pthread.h>
#include <math.h>

#include "directory.h"
#include "thread.h"

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
