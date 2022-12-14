#include "directory.h"
#include <pthread.h>
#include <fnmatch.h>
#include <sys/stat.h>

// taken and modified from https://stackoverflow.com/a/5804935/11902943
static const char* gnu_basename(const char* path)
{
  char* base = strrchr(path, '/');
  return base ? base+1 : path;
}

////// Helpers for globbing and searching ////////
static inline int match_pattern_filename(const char* pattern, const char* filepath)
{
  return fnmatch(pattern, gnu_basename(filepath), 0x0);
}

static int match_file_type(const file_type_t type, const mode_t f_mode)
{
  switch (type)
  {
  case REGULAR:
    if (S_ISREG(f_mode))
      return 0;
    break;
  case DIRECTORY:
    if (S_ISDIR(f_mode))
      return 0;
    break;
  }
  return 1;
}

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

  // Filter by filename
  if (ch_opts->ch_filename && match_pattern_filename(ch_opts->ch_filename, filepath) != 0)
    return;
  // Filter by file type
  if (ch_opts->ch_type && match_file_type(ch_opts->ch_type, sbuf.st_mode) != 0)
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
