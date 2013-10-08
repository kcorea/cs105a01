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

static char *slist_tag = "struct st_slist";
static char *slist_node_tag = "struct st_node"; 

struct st_slist {
   char *tag;
   slist_node head;
   int blkct;
   slist_node stack_top;
};

struct st_node {
   char *tag;
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
   slist_node slink;
};

bool is_slist (slist_ref slist) {
   return slist != NULL && slist->tag == slist_tag;
} 

bool is_slistnode (slist_node node) {
   return node != NULL && node->tag == slist_node_tag;
}

bool is_hidden (slist_node node) {
   assert (is_slistnode (node) && node->fname != NULL);
   return node->fname[0] == '.';
} 

bool is_dir (slist_node node) {
   assert (is_slistnode (node));
   return node->permissions[0] == 'd';
}

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
   return strdup (buffer);
}

char *get_uidstr (uid_t uid) {
   struct passwd *pwd;
   pwd = getpwuid (uid); // error check
   return strdup (pwd->pw_name);
}

char *get_gidstr (gid_t gid) {
   struct group *grp;
   grp = getgrgid (gid); // handle error
   return strdup (grp->gr_name);
}

char *get_mtimestr (time_t mtime) {
   char buffer[16];
   struct tm *mtinfo = localtime (&mtime);
   strftime (buffer, 16, "%b %e %H:%M", mtinfo);
   return strdup (buffer);
}

char *tolower_str (char *string) {
   int len = strlen (string);
   char *lstring = calloc (len+1, sizeof (char));
   strcpy (lstring, string);
   for (int itor = 0; itor < len; ++itor) {
      lstring[itor] = tolower (lstring[itor]);
   }
   return lstring;
}

slist_node new_slistnode (struct stat *fs, char *filename) {
   assert (fs != NULL && filename != NULL);
   slist_node new = malloc (sizeof (struct st_node));
   assert (new != NULL);

   new->tag = slist_node_tag;
   new->permissions = get_pstr (fs->st_mode);
   new->uid = get_uidstr (fs->st_uid);
   new->gid = get_gidstr (fs->st_gid);
   new->mtime = get_mtimestr (fs->st_mtime);
   new->fname = strdup (filename);
   new->lfname = tolower_str (filename);
   new->blocks = fs->st_blocks;
   new->hlinks = fs->st_nlink;
   new->size = fs->st_size;
   new->link = NULL;   
   new->slink = NULL;
   DEBUGF ('n', "node=%p; fname=%s; lfname=%s\n", 
            new, new->fname, new->lfname);
   return new;
}

slist_ref new_slist (void) {
   slist_ref new = malloc (sizeof (struct st_slist));
   assert (new != NULL);

   new->tag = slist_tag;
   new->head = NULL;
   new->blkct = 0;
   new->stack_top = NULL;
   return new;
}

void insert_slist (slist_ref slist, slist_node newnode) {
   assert (is_slist (slist) && is_slistnode (newnode));
   slist_node prev = NULL;
   slist_node curr = slist->head;
   // Find insertion position.
   while (curr != NULL) {
      if (strcmp (curr->lfname, newnode->lfname) > 0) break;
      prev = curr;
      curr = curr->link;
   }
   // Do the insertion.
   newnode->link = curr;
   if (prev == NULL) slist->head = newnode;
                else prev->link = newnode;

   if (!is_hidden (newnode)) slist->blkct += newnode->blocks;
}

bool has_empty_dirstack (slist_ref slist) {
   assert (is_slist (slist));
   return slist->stack_top == NULL;
}

char *pop_slistdir (slist_ref slist) {
   assert (is_slist (slist));
   assert (!has_empty_dirstack (slist));
   slist_node tmp = slist->stack_top;
   char *dir = tmp->fname;
   slist->stack_top = slist->stack_top->slink;
   return dir;
}

void push_slistdir (slist_ref slist, slist_node node) {
   assert (is_slist (slist));
   assert (is_slistnode (node));
      DEBUGF ('f',"");
   node->slink = slist->stack_top;
   slist->stack_top = node;
}

void stack_slistdir (slist_ref slist) {
   assert (is_slist (slist));

   slist_node curr = slist->head;
   for (;;) {
      if (curr == NULL) break;
      DEBUGF ('f',"");
      if (is_dir (curr)) push_slistdir (slist, curr);
      curr = curr->link;
   }
}

void free_slistnode (slist_node node) {
   assert (is_slistnode (node));
   free (node->permissions);
   free (node->gid);
   free (node->uid);
   free (node->mtime);
   free (node->fname);
   free (node->lfname);
   free (node);   
}

void free_slist (slist_ref slist) {
   assert (is_slist (slist));
   slist_node old = slist->head;
   for (;;) {
      if (old == NULL) break;
      slist->head = slist->head->link;
      free_slistnode (old);
      old = slist->head;
   }
   free (slist);
}

void print_slist_long (slist_ref slist) {
   printf ("total %d\n", slist->blkct/2);  // 1024B block size
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
