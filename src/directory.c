#include "directory.h"
#include <dirent.h>
#include <sys/stat.h>
#include <pthread.h>

static void append_to_path(const char* rel_path, const char* head, char* buf)
{
  char* format = "%s/%s";
  if (is_root_directory(rel_path)) format = "%s%s";

  sprintf(buf, format, rel_path, head);
}

unsigned int get_num_files(DIR* dirp)
{
  struct dirent* entry;
  unsigned int count = 0;

  while ((entry=readdir(dirp)) != NULL)
  {
    if (is_valid_dir(entry->d_name))
      ++count;
  }

  //rewinddir(dirp);
  return count;
}

DIR* open_dir(const char* dir_path)
{
  DIR* dir = opendir(dir_path);
  
  if (!dir)
  {
    fprintf(stderr, "ERROR on opendir '%s' %d: %s\n", dir_path, errno, strerror(errno));
    exit(EXIT_FAILURE);
  }
  return dir;
}

int close_dir(DIR* dir)
{
  int ret;
  if ((ret=closedir(dir)) < 0)
    fprintf(stderr, "ERROR on closedir %d: %s\n", errno, strerror(errno));

  return ret;
}

static pthread_mutex_t mutex;

void output_file(const char* filepath, pthread_mutex_t* mutex, const CHASE_OPTS* ch_opts)
{
  struct stat sbuf;
  if (lstat(filepath, &sbuf))
  {
    fprintf(stderr, "ERROR on lstat(%s) %d: %s\n", filepath, errno, strerror(errno));
    exit(EXIT_FAILURE);
  }

  // Filter file
  if (ch_opts->ch_filename && strstr(filepath, ch_opts->ch_filename) == NULL)
    return;

  // Finally print the searched file
  pthread_mutex_lock(mutex);
  fprintf(stdout, "Thread(%lu): %s\n", pthread_self(), filepath);
  pthread_mutex_unlock(mutex);
}

void traverse_dir(DIR* dirp, const char* rel_path, int level, const unsigned int limit, const CHASE_OPTS* ch_opts)
{
  struct dirent* entry;
  unsigned int count = 0;
  
  char filepath[PATH_MAX + 1];

  while ((entry=readdir(dirp)) != NULL && count < limit)
  {
    if (!is_valid_dir(entry->d_name))
      continue;

    if (level == 0) ++count;

    append_to_path(rel_path, entry->d_name, filepath);

    // Filter |> Output a file to stdout
    output_file(filepath, &mutex, ch_opts);

    if (entry->d_type & DT_DIR)
    {
      DIR* n_dirp = open_dir(filepath);
      traverse_dir(n_dirp, filepath, level+1, limit, ch_opts);
      close_dir(n_dirp);
    }
  }
}
