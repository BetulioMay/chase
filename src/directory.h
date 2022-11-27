#ifndef _DIRECTORY_H
#define _DIRECTORY_H

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <assert.h>
#include <pthread.h>

inline int is_valid_dir(const char* dir_name)
{
  return (!strcmp(dir_name, ".") || !strcmp(dir_name, "..") ? 0 : 1);
}

DIR* open_dir(const char* dir_path);

int close_dir(DIR* dir);

void traverse_dir(DIR* dirp, const char* rel_path, int level, const unsigned int limit);

unsigned int get_num_files(DIR* dirp);

#endif  /* _DIRECTORY_H */
