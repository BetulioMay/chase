#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>

static DIR* open_dir(const char* dir_path)
{
  DIR* dirent = opendir(dir_path);
  
  if (!dirent)
    fprintf(stderr, "ERROR on opendir %d: %s\n", errno, strerror(errno));
  return dirent;
}

int main(int argc, char** argv)
{
  if (argc < 2)
  {
    fprintf(stderr, "ERROR: bad arguments\n");
    exit(EXIT_FAILURE);
  }

  DIR* dirent = open_dir(argv[1]);

  return 0;
}
