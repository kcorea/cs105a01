#include <assert.h>
#include <ctype.h>
#include <grp.h>
#include <pwd.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include "debugf.h"
#include "slist.h"


struct st_slist {
   slist_node head;
   int blkct;
};

struct st_node {
   char *permissions;
   char *uid;
   char *gid;
   char *mtime;
   char *fname;
   char *lfname;
   int blocks;
   int hlinks;
   int size;
   slist_node link;
};

char *get_pstr (mode_t mode) {
   char buffer[16];
   strcpy (buffer, "");
   strcat (buffer, (S_ISDIR (mode)) ? "d" : "-");
   strcat (buffer, (mode & S_IRUSR) ? "r" : "-");
   strcat (buffer, (mode & S_IWUSR) ? "w" : "-");
   strcat (buffer, (mode & S_IXUSR) ? "x" : "-");
   strcat (buffer, (mode & S_IRGRP) ? "r" : "-");
   strcat (buffer, (mode & S_IWGRP) ? "w" : "-");
   strcat (buffer, (mode & S_IXGRP) ? "x" : "-");
   strcat (buffer, (mode & S_IROTH) ? "r" : "-");
   strcat (buffer, (mode & S_IWOTH) ? "w" : "-");
   strcat (buffer, (mode & S_IXOTH) ? "x" : "-");
  
   //STUBPRINTF ("%s\n", buffer);

   return strdup (buffer);
}

char *get_uidstr (uid_t uid) {
   struct passwd *pwd;
   pwd = getpwuid (uid); // error check

   //STUBPRINTF ("%s\n", pwd->pw_name);  

   return strdup (pwd->pw_name);
}

char *get_gidstr (gid_t gid) {
   struct group *grp;
   grp = getgrgid (gid); // handle error

   //STUBPRINTF ("%s\n", grp->gr_name);  

   return strdup (grp->gr_name);
}

char *get_mtimestr (time_t mtime) {
   char buffer[16];
   struct tm *mtinfo = localtime (&mtime);
   strftime (buffer, 16, "%b %e %H:%M", mtinfo);

   //STUBPRINTF ("%s\n", buffer);  

   return strdup (buffer);
}

char *tolower_str (char *string) {
   int len = strlen (string);
   char *lstring = calloc (len+1, sizeof (char));
   strcpy (lstring, string);
   for (int itor = 0; itor < len; ++itor) {
      lstring[itor] = tolower (lstring[itor]);
   }
   return strdup (lstring);
}

slist_node new_slistnode (struct stat *fs, char *filename) {
   slist_node new = malloc (sizeof (struct st_node));
   assert (new != NULL);

   new->permissions = get_pstr (fs->st_mode);
   new->uid = get_uidstr (fs->st_uid);
   new->gid = get_gidstr (fs->st_gid);
   new->mtime = get_mtimestr (fs->st_mtime);
   new->fname = strdup (filename);
   new->lfname = tolower_str (filename);
   new->blocks = fs->st_blocks;
   new->hlinks = fs->st_nlink;
   new->size = fs->st_size;

   //STUBPRINTF ("hlinks=%d; size=%d\n", new->hlinks, new->size);

   new->link = NULL;   
   return new;
}

slist_ref new_slist (void) {
   slist_ref new = malloc (sizeof (struct st_slist));
   assert (new != NULL);
   new->head = NULL;
   new->blkct = 0;
   return new;
}

void insert_slist (slist_ref slist, slist_node newnode) {
   slist_node prev = NULL;
   slist_node curr = slist->head;
   // Find insertion position.
   while (curr != NULL) {
      if (strcmp (curr->lfname, newnode->lfname ) > 0) break;
      prev = curr;
      curr = curr->link;
   }
   // Do the insertion.
   newnode->link = curr;
   if (prev == NULL) slist->head = newnode;
                else prev->link = newnode;
   slist->blkct += newnode->blocks;
}

void free_slist (slist_ref slist) {
   STUBPRINTF ("free all nodes\n");
}

void print_slist_long (slist_ref slist) {
   printf ("total %d\n", slist->blkct);
   slist_node curr = slist->head;
   for (;;) {
      if (curr == NULL) break;
      printf ("%s %2d %s %s %5d %s %s\n", 
              curr->permissions, curr->hlinks, curr->uid, 
              curr->gid, curr->size, curr->mtime, curr->fname);
      curr = curr->link;
   }
}

void print_slist_short (slist_ref slist) {
   slist_node curr = slist->head;
   for (;;) {
      if (curr == NULL) break;
      printf ("%s\n", curr->fname);
      curr = curr->link;
   }
}
