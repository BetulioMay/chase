#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <linux/limits.h>
#include <sys/stat.h>

static void parse_dir(char* dir_arg)
{
  // Get rid of trailing '/'
  unsigned long size = strlen(dir_arg) + 1;

  if (dir_arg[size-2] == '/')
  {
    dir_arg[size-2] = '\0';
  }
}

static void append_to_path(const char* rel_path, const char* head, char* buf)
{
  sprintf(buf, "%s/%s", rel_path, head);
}

static DIR* open_dir(const char* dir_path)
{
  DIR* dir = opendir(dir_path);
  
  if (!dir)
  {
    fprintf(stderr, "ERROR on opendir '%s' %d: %s\n", dir_path, errno, strerror(errno));
    exit(EXIT_FAILURE);
  }
  return dir;
}

static int close_dir(DIR* dir)
{
  int ret;
  if ((ret=closedir(dir)) < 0)
    fprintf(stderr, "ERROR on closedir %d: %s\n", errno, strerror(errno));

  return ret;
}

static inline int is_valid_dir(const char* dir_name)
{
  return (!strcmp(dir_name, ".") || !strcmp(dir_name, "..") ? 0 : 1);
}

static void traverse_dir(DIR* dirp, const char* rel_path)
{
  struct dirent* dirent;
  
  char filepath[PATH_MAX + 1];
  struct stat sbuf;
  while ((dirent=readdir(dirp)) != NULL)
  {
    if (!is_valid_dir(dirent->d_name))
      continue;

    append_to_path(rel_path, dirent->d_name, filepath);
    fprintf(stdout, "%s\n", filepath);

    lstat(filepath, &sbuf);

    if (S_ISDIR(sbuf.st_mode))
    {
      DIR* n_dirp = open_dir(filepath);
      traverse_dir(n_dirp, filepath);
    }
  }
}

int main(int argc, char** argv)
{
  if (argc < 2)
  {
    fprintf(stderr, "ERROR: bad arguments\n");
    exit(EXIT_FAILURE);
  }

  // Parse arguments
  parse_dir(argv[1]);

  // Open directory
  DIR* dirp = open_dir(argv[1]);

  traverse_dir(dirp, argv[1]);

  // Close directory pointer
  close_dir(dirp);

  return 0;
}
