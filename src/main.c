#define _GNU_SOURCE
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/limits.h>

#include <pthread.h>
#include <math.h>
#include <argp.h>

#include "directory.h"
#include "thread.h"
#include "args.h"

/***** ARGP Global Variables *****/
// Program version
const char* argp_program_version = 
  "chase 0.1.1";

// Bug address
const char* program_bug_address = 
  "<https://github.com/BetulioMay/chase/issues>";

// Documentation
static char doc[] = 
  "Chase -- simple concurrent file seeker";

// Program Arguments
static char args_doc[] = "[initial_dir...]";

static struct argp_option options[] = {
  {"name",  'n', "NAME",      0,  "Search by filename", 0 },
  {"type",    't', "TYPE",      0,  FILE_TYPE_DESC, 0 },
  { 0 }
};

struct arguments
{
  char* args[1];                /* arg1 */
  char* name;
  file_type_t type;
};

static file_type_t validate_file_type(const char* in_type)
{
  if (strlen(in_type) != 1)
  {
    fprintf(stderr, "ERROR: bad arg option value %s\n", in_type);
    exit(EXIT_FAILURE);
  }

  file_type_t type;

  switch(in_type[0])
  {
  case 'f':
    type = REGULAR;
    break;
  case 'd':
    type = DIRECTORY;
    break;
  default:
    fprintf(stderr, "ERROR: type %c not supported\n", in_type[0]);
    exit(EXIT_FAILURE);
    break;
  }
  return type;
}

static error_t parse_opt (int key, char *arg, struct argp_state *state)
{
  /* Get the input argument from argp_parse, which we
     know is a pointer to our arguments structure. */
  struct arguments *arguments = state->input;

  switch (key)
    {
    case 'n':
      arguments->name = arg;
      break;
    case 't':
      arguments->type = validate_file_type(arg);
      break;

    case ARGP_KEY_ARG:
      if (state->arg_num > 2)
        /* Too many arguments. */
        argp_usage(state);

      arguments->args[state->arg_num] = arg;

      break;

    case ARGP_KEY_END:
      if (state->arg_num < 1)
        /* Not enough arguments. */
        argp_usage(state);
      break;

    default:
      return ARGP_ERR_UNKNOWN;
    }
  return 0;
}
// Parser
static struct argp argp = { options, parse_opt, args_doc, doc, 0, 0, 0 };
/************************/

static void parse_dir_arg(char* dir_arg)
{
  if (is_root_directory(dir_arg)) return;

  // Get rid of trailing '/'
  unsigned long size = strlen(dir_arg) + 1;

  // remove trailing '/' if not root directory
  if (dir_arg[size-2] == '/' && strlen(dir_arg) > 1)
  {
    dir_arg[size-2] = '\0';
  }
}


int main(int argc, char** argv)
{
  struct arguments arguments;
  arguments.name = 0;
  arguments.type = REGULAR;

  // Parse arguments
  argp_parse(&argp, argc, argv, 0, 0, &arguments);

  // Init options
  CHASE_OPTS ch_opts;
  ch_opts.ch_filename = arguments.name;
  ch_opts.ch_type = arguments.type;

  parse_dir_arg(arguments.args[0]);
  char* initial_dir = arguments.args[0];

  // Open directory
  DIR* dirp = open_dir(initial_dir);
 
  // Init threads
  THREAD_ARGS args = {
    .tid = -1,    // -1 === no id assigned
    .dirp = dirp,
    .initial_dir = initial_dir,
    .ch_opts = &ch_opts
  };

  pthread_t threads[MAX_NUM_THREADS];
  int num_threads = create_threads(threads, &args);

  // Start chasing
  //traverse_dir(dirp, argv[1]);

  for (int i = 0; i < num_threads; ++i)
  {
    void* status;
    int t_error = pthread_join(threads[i], &status);
    if (t_error)
      fprintf(stderr, "ERROR on pthread_join %d\n", t_error);
  }

  // Close directory
  close_dir(dirp);

  return 0;
}
