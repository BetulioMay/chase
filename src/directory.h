#ifndef _DIRECTORY_H
#define _DIRECTORY_H

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <assert.h>

typedef enum
{
  REGULAR,
  DIRECTORY
} file_type_t;

typedef struct _chase_options
{
  char* ch_filename;
  file_type_t ch_type;
} CHASE_OPTS;

inline int is_root_directory(const char* dir)
{
  return (strcmp(dir, "/") == 0 ? 1 : 0);
}

inline int is_valid_dir(const char* dir_name)
{
  return (!strcmp(dir_name, ".") || !strcmp(dir_name, "..") ? 0 : 1);
}

DIR* open_dir(const char* dir_path);

int close_dir(DIR* dir);

void traverse_dir(DIR* dirp, const char* rel_path, int level, const unsigned int limit, const CHASE_OPTS* ch_opts);

void output_file(const char* filename, pthread_mutex_t* mutex, const CHASE_OPTS* ch_opts);

unsigned int get_num_files(DIR* dirp);

#endif  /* _DIRECTORY_H */
