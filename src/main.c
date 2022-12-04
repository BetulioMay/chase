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
  if (is_root_directory(dir_arg)) return;

  // Get rid of trailing '/'
  unsigned long size = strlen(dir_arg) + 1;

  // remove trailing '/' if not root directory
  if (dir_arg[size-2] == '/' && strlen(dir_arg) > 1)
  {
    dir_arg[size-2] = '\0';
  }
}

static file_type_t validate_file_type(const char* in_type)
{
  if (strlen(in_type) != 1)
  {
    fprintf(stderr, "ERROR: bad arg option value %s\n", in_type);
    exit(EXIT_FAILURE);
  }

  file_type_t type;

  switch(in_type[0])
  {
  case 'f':
    type = REGULAR;
    break;
  case 'd':
    type = DIRECTORY;
    break;
  default:
    fprintf(stderr, "ERROR: type %c not supported\n", in_type[0]);
    exit(EXIT_FAILURE);
    break;
  }
  return type;
}

int main(int argc, char** argv)
{
  if (argc < 2)
  {
    fprintf(stderr, "ERROR: bad arguments\n");
    exit(EXIT_FAILURE);
  }

  // Parse search directory
  parse_dir_arg(argv[1]);
  char* initial_dir = argv[1];

  // Parse options
  CHASE_OPTS ch_opts;
  char ch;
  while ((ch=getopt(argc, argv, "n:t:")) != -1)
  {
    switch(ch)
    {
      case 'n':
        ch_opts.ch_filename = optarg;
        break;
      case 't':
        ch_opts.ch_type = validate_file_type(optarg);
        break;
      default:break;
    }
  }

  // Open directory
  DIR* dirp = open_dir(initial_dir);
 
  // Init threads
  THREAD_ARGS args = {
    .tid = -1,    // -1 === no id assigned
    .dirp = dirp,
    .initial_dir = initial_dir,
    .ch_opts = &ch_opts
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
