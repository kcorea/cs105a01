#include <dirent.h>
#include <errno.h>
#include <getopt.h>
#include <libgen.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include "debugf.h"
#include "dqueue.h"
#include "slist.h"

#define WORKINGDIR "."
#define PARENTDIR  ".."
#define ROOTDIR "/"

#define MAX_BUFFER_SIZE 1024

struct prog_flags {
   bool long_listing;
   bool recursive;
   char *pathname;
};

char *execname = NULL;
int exit_status = EXIT_SUCCESS;
struct prog_flags flags = {false, false, NULL};
queue_ref queue = NULL;

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

void pathstr (char *buffer, char *path, char *fname) {
   char *dirc = strdup (path);
   char *basec = strdup (path);
   char *dname = dirname (dirc);
   char *bname = basename (basec);

   if (strcmp (dname, WORKINGDIR) == 0) {
      if (strcmp (bname, WORKINGDIR) == 0) {
         strcpy (buffer, WORKINGDIR);
      }else if (strcmp (bname, PARENTDIR) == 0) {
         strcpy (buffer, PARENTDIR);
         strcpy (buffer, "/");
         strcpy (buffer, bname);
      }else {
         strcpy (buffer, WORKINGDIR);
         strcpy (buffer, "/");
         strcpy (buffer, bname);   
      }
      strcat (buffer, "/");
   }else if (strcmp (dname, ROOTDIR) == 0) {
      if (strcmp (bname, ROOTDIR) == 0) {
         strcpy (buffer, ROOTDIR);
      }else {
         strcpy (buffer, ROOTDIR);
         strcat (buffer, "/");
         strcat (buffer, bname);
         strcat (buffer, "/");
      }
   }else {
      strcpy (buffer, dname);
      strcat (buffer, "/");
      strcat (buffer, bname);
      strcat (buffer, "/");
   }
   strcat (buffer, fname);
   free (dirc);
   free (basec);
}

void insert (slist_ref slist, char *ename, char *path) {
   char buffer[MAX_BUFFER_SIZE];
   pathstr (buffer, path, ename);
   struct stat fs;
   int stat_rc = stat (buffer, &fs);
   if (stat_rc >= 0) {
      slist_node node = new_slistnode (&fs, ename);

      if (! is_hidden (node)) {
         if (is_dir (node) && flags.recursive == true) {
            
            insert_queue (queue, buffer);
         }
         insert_slist (slist, node);
      }else {
         free_slistnode (node);
      }

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
         insert (slist, ent->d_name, path);
      }
      closedir (dir);
   }else {
      print_error (path, strerror (errno));
   }
   print_directory (slist);
   free_slist (slist);
}

void recursive_list (char *path) {
   queue = new_queue ();
   insert_queue (queue, path);
   for (;;) {
      char *dir = remove_queue (queue);
      printf ("%s:\n", dir);
      list (dir);
      if (is_empty_queue (queue)) break;
      printf ("\n");
   }
   free_queue (queue);
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

   // recurse over directory 
   if (flags.recursive == true) {
      if (flags.pathname == NULL) {
         // no pathname specified, list working directory
         recursive_list (WORKINGDIR);
      }else {
         recursive_list (flags.pathname);
      }
   }else {
      if (flags.pathname == NULL) {
         // no pathname specified, list working directory
         list (WORKINGDIR);
      }else {
         list (flags.pathname);
      }
   }
}
