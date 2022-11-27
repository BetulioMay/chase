#include "directory.h"

inline static int is_valid_dir(const char* dir_name)
{
  return (!strcmp(dir_name, ".") || !strcmp(dir_name, "..") ? 0 : 1);
}

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
    if (is_valid_dir(entry->d_name))
      ++count;
  }

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
