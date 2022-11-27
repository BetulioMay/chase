#include "thread.h"
#include "directory.h"
#include <math.h>

void* thread_func(void* args_void)
{
  THREAD_ARGS* args = args_void;

  //printf("dir for thread(%u): %lu\n", args->tid, telldir(args->dirp));

  traverse_dir(args->dirp, args->initial_dir, 0, args->num_tasks);

  close_dir(args->dirp);
  free(args);

  pthread_exit(NULL);
  return NULL;
}

unsigned int create_threads(pthread_t* threads, THREAD_ARGS* common_args)
{
  const unsigned int NUM_FILES = get_num_files(common_args->dirp);
  const unsigned int DIRS_PER_THREAD = ceil((double)NUM_FILES / (double)MAX_NUM_THREADS);
  printf("Number of files: %u\n", NUM_FILES);
  printf("dirs per thread: %u\n", DIRS_PER_THREAD);

  unsigned int num_threads = ceil((NUM_FILES*1.0) / (DIRS_PER_THREAD*1.0));

  // Only use max num threads
  if (num_threads > MAX_NUM_THREADS)
    num_threads = MAX_NUM_THREADS;

  // Assign workload to threads
  DIR** dirs = (DIR**)malloc(sizeof(DIR*) * MAX_NUM_THREADS);
  assign_workload(dirs, num_threads, common_args->initial_dir, DIRS_PER_THREAD);

  printf("Number of threads to create: %u\n", num_threads);

  for (unsigned int i = 0; i < num_threads && i < MAX_NUM_THREADS; ++i)
  {
    THREAD_ARGS* thread_args = (THREAD_ARGS*)malloc(sizeof(*common_args));

    thread_args->tid = i;
    thread_args->dirp = dirs[i];
    thread_args->initial_dir = common_args->initial_dir;
    thread_args->num_tasks = DIRS_PER_THREAD;

    int t_error = pthread_create(&threads[i], NULL, thread_func, (void*)thread_args);

    if (t_error)
      fprintf(stderr, "ERROR on pthread_create\n");
  }

  // Not going to use it anymore
  free(dirs);

  return num_threads;
}

void assign_workload(DIR** dirs, int num_workers, const char* dirpath, unsigned int dirs_thread)
{
  assert(dirs != NULL);

  unsigned int count = 0;
  struct dirent* dummy;

  for (int i = 0; i < num_workers; ++i)
  {
    dirs[i] = open_dir(dirpath);

    // NOTE: If turns out that dirp=0 is not valid location, delete this if-sentence
    if (i == 0)
    {
      continue;
    }

    while ((dummy=readdir(dirs[i])) != NULL)
    {
      if (count == i * dirs_thread)
        break;

      // Parent and current dir as if they don't exist... or count
      count += (is_valid_dir(dummy->d_name) ? 1 : 0);
    }
    count = 0;
  }
}

