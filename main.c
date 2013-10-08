#include <assert.h>
#include <dirent.h>
#include <errno.h>
#include <getopt.h>
#include <libgen.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include "debugf.h"
#include "dstack.h"
#include "slist.h"

#define WORKINGDIR "."
#define PARENTDIR  ".."
#define ROOTDIR "/"

#define MAX_BUFFER_SIZE 1024

struct prog_flags {
   bool long_listing;  
   bool recursive;
   char **pathname;
   int opct;
};

char *execname = NULL;
int exit_status = EXIT_SUCCESS;
struct prog_flags flags = {false, false, NULL, 0};
stack_ref stack = NULL;

void print_error (char *object, char *message) {
   fflush (NULL);
   fprintf (stderr, "%s: %s: %s\n", execname, object, message);
   fflush (NULL);
   exit_status = EXIT_FAILURE;
}

void print_directory (slist_ref slist) {
   if (flags.long_listing == true) {
      print_slist_long (slist);
   }else {
      print_slist_short (slist);
   }
}

void catlist (char *buffer, char *format, ...) {
   va_list args;
   va_start (args, format);
   vsprintf (buffer, format, args);
   va_end (args);
}

void pathstr (char *buffer, char *path, char *fname) {
   char *dirc = strdup (path);
   char *basec = strdup (path);
   char *dname = dirname (dirc);
   char *bname = basename (basec);

   if (strcmp (dname, WORKINGDIR) == 0) {
      if (strcmp (bname, WORKINGDIR) == 0) {
         catlist (buffer, "%s/%s" , WORKINGDIR, fname);
      }else if (strcmp (bname, PARENTDIR) == 0) {
         catlist (buffer, "%s/%s", PARENTDIR, fname);
      }else {
         catlist (buffer, "%s/%s/%s", WORKINGDIR, bname, fname);
      }
   }
   else if (strcmp (dname, ROOTDIR) == 0) {

      if (strcmp (bname, ROOTDIR) == 0) {
         catlist (buffer, "/%s", fname);
      }else {
         catlist (buffer, "/%s/%s", bname, fname);
      }
   }
   else {
      catlist (buffer, "%s/%s/%s", dname, bname, fname);
   }
   free (dirc);
   free (basec);
}

void push_subdir (slist_ref slist, char *path) {
   stack_slistdir (slist);         
   for (;;) {
       if (has_empty_dirstack (slist)) break;
      DEBUGF ('f',"");
       char *ename = pop_slistdir (slist);
       char buffer[MAX_BUFFER_SIZE];
       pathstr (buffer, path, ename);
       push_stack (stack, buffer);
   }
}

void insert (slist_ref slist, char *path, char *ename) {
   char buffer[MAX_BUFFER_SIZE];
   pathstr (buffer, path, ename);
   struct stat fs;
   int stat_rc = stat (buffer, &fs);
   if (stat_rc >= 0) {
      slist_node node = new_slistnode (&fs, ename);

      if (! is_hidden (node)) insert_slist (slist, node);
                         else free_slistnode (node);
      
   }else {
      print_error (ename, strerror (errno));
   }
}

void list (char *path) {
   DIR *dir = opendir (path);
   struct dirent *ent;
   slist_ref slist = new_slist ();

   if (dir != NULL) {
      for (;;) {
         ent = readdir (dir);
         if (ent == NULL) break;
         insert (slist, path, ent->d_name);
      }
      closedir (dir);
      if (flags.recursive) push_subdir (slist, path);
      if (flags.opct > 1 && !flags.recursive) 
         printf ("%s:\n", path);
      print_directory (slist);
   
   }else {
      print_error (path, strerror (errno));
   }
   free_slist (slist);
}

void recursive_list (char *path) {
   stack = new_stack ();
   push_stack (stack, path);
   for (;;) {
      char *dir = pop_stack (stack);
      printf ("%s:\n", dir);
      list (dir);
      if (is_empty_stack (stack)) break;
      printf ("\n");
   }
   free_stack (stack);
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
         case 'R': flags.recursive = true;
                   break;
         case '@': set_debugflags (optarg);
                   break;
         default : sprintf (optopt_str, "-%c", optopt); 
                   print_error (optopt_str, "invalid option");
                   exit (exit_status);
      }
   }
   // collect any operands in argv
   if (optind < argc) {
      flags.opct = argc - optind;    
      flags.pathname = calloc (flags.opct, sizeof (char*));
      assert (flags.pathname != NULL);
      for (int itor = optind; itor < argc; ++itor) {
         flags.pathname[itor - optind] = argv[itor];
      }
   }
}

int main (int argc, char **argv) {
   execname = basename (argv[0]);
   set_execname (execname);
   set_options (argc, argv); 

   if (flags.recursive == true) {
      if (flags.opct == 0) { recursive_list (WORKINGDIR); }
      else if (flags.opct == 1) { recursive_list (flags.pathname[0]); }
      else {
         for (int ind = 0; ind < flags.opct; ++ind) {
            recursive_list (flags.pathname[ind]);
            printf ("\n");
         }
      }
   }else {
      if (flags.pathname == 0) { list (WORKINGDIR); }
      else if (flags.opct == 1) { list (flags.pathname[0]); }
      else {
         for (int ind = 0; ind < flags.opct; ++ind) {
            list (flags.pathname[ind]);
            printf ("\n");
         }
      }      
   }
   return (exit_status);
}
