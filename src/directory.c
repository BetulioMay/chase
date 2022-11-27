#include "directory.h"
#include <dirent.h>

inline static int is_valid_dir(const char* dir_name)
{
  return (!strcmp(dir_name, ".") || !strcmp(dir_name, "..") ? 0 : 1);
}

static inline void append_to_path(const char* rel_path, const char* head, char* buf)
{
  sprintf(buf, "%s/%s", rel_path, head);
}

void assign_workload(DIR** dirs, int num_workers, const char* dirpath)
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
      if ((int)count == i * DIRS_PER_THREAD)
        break;

      // Parent and current dir as if they don't exist... or count
      count += (is_valid_dir(dummy->d_name) ? 1 : 0);
    }
    count = 0;
  }
}

unsigned int get_num_files(DIR* dirp)
{
  struct dirent* entry;
  unsigned int count = 0;

  while ((entry=readdir(dirp)) != NULL)
  {
    //printf("%s: %lu\n", entry->d_name, telldir(dirp));
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

void traverse_dir(DIR* dirp, const char* rel_path)
{
  struct dirent* entry;
  
  char filepath[PATH_MAX + 1];
  struct stat sbuf;
  while ((entry=readdir(dirp)) != NULL)
  {
    if (!is_valid_dir(entry->d_name))
      continue;

    append_to_path(rel_path, entry->d_name, filepath);
    fprintf(stdout, "%s\n", filepath);

    if (lstat(filepath, &sbuf))
    {
      fprintf(stderr, "ERROR on lstat %d: %s\n", errno, strerror(errno));
      exit(EXIT_FAILURE);
    }

    if (S_ISDIR(sbuf.st_mode))
    {
      DIR* n_dirp = open_dir(filepath);
      traverse_dir(n_dirp, filepath);
    }
  }
}
