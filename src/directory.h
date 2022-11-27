#ifndef _DIRECTORY_H
#define _DIRECTORY_H

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <assert.h>

#define DIRS_PER_THREAD 5

DIR* open_dir(const char* dir_path);

int close_dir(DIR* dir);

void traverse_dir(DIR* dirp, const char* rel_path);

unsigned int get_num_files(DIR* dirp);

void assign_workload(DIR** dirs, int num_workers, const char* dirpath);

#endif  /* _DIRECTORY_H */
