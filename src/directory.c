#include "directory.h"
#include <dirent.h>
#include <sys/stat.h>

static inline void append_to_path(const char* rel_path, const char* head, char* buf)
{
  sprintf(buf, "%s/%s", rel_path, head);
}


unsigned int get_num_files(DIR* dirp)
{
  struct dirent* entry;
  unsigned int count = 0;

  while ((entry=readdir(dirp)) != NULL)
  {
    printf("%s: %lu\n", entry->d_name, telldir(dirp));
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

void traverse_dir(DIR* dirp, const char* rel_path, int level, const unsigned int limit)
{
  struct dirent* entry;
  unsigned int count = 0;
  
  char filepath[PATH_MAX + 1];
  struct stat sbuf;

  while ((entry=readdir(dirp)) != NULL && count < limit)
  {
    if (!is_valid_dir(entry->d_name))
      continue;

    if (level == 0) ++count;

    append_to_path(rel_path, entry->d_name, filepath);

    pthread_mutex_lock(&mutex);
    fprintf(stdout, "Thread(%lu): %s\n", pthread_self(), filepath);
    pthread_mutex_unlock(&mutex);

    if (lstat(filepath, &sbuf))
    {
      fprintf(stderr, "ERROR on lstat %d: %s\n", errno, strerror(errno));
      exit(EXIT_FAILURE);
    }

    if (S_ISDIR(sbuf.st_mode))
    {
      // TODO: errno 24. Too many files opened because of this
      DIR* n_dirp = open_dir(filepath);
      traverse_dir(n_dirp, filepath, level+1, limit);
      close_dir(n_dirp);
    }
  }
}
