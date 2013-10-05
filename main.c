#include <getopt.h>
#include <libgen.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "debugf.h"


struct prog_flags {
   bool long_listing;
   bool recursive;
   char *pathname;
};

char *execname = NULL;
int exit_status = EXIT_SUCCESS;
struct prog_flags flags = {false, false, NULL};

void print_error (char *object, char *message) {
   fflush (NULL);
   fprintf (stderr, "%s: %s: %s\n", execname, object, message);
   fflush (NULL);
   exit_status = EXIT_FAILURE;
}

void set_options (int argc, char **argv) {
   static struct option long_options[] =
   {
      {"list", no_argument, NULL, 'l'},
      {"recurse", no_argument, NULL, 'R'},
      {"debug", required_argument, NULL, '@'},
      {NULL, 0, NULL, 0}
   };

   opterr = false;
   for (;;) {
      int option = getopt_long (argc, argv, "lR@:", long_options, NULL);
      if (option == EOF) break;
      switch (option) {
         char optopt_str[16]; // used for printing error
         case 'l': flags.long_listing = true;
                   break;
         case 'R': flags.long_listing = true;
                   break;
         case '@': set_debugflags (optarg);
                   break;
         default : sprintf (optopt_str, "-%c", optopt); 
                   print_error (optopt_str, "invalid option");
                   break;
      }
   }
   // program only takes one directory argument
   if (optind < argc) flags.pathname = argv[optind];
}

int main (int argc, char **argv) {
   execname = basename (argv[0]);
   set_execname (execname);
   set_options (argc, argv); 
}
