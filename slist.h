#ifndef __SLIST_H__
#define __SLIST_H__

#include <sys/stat.h>

typedef struct st_slist *slist_ref;
typedef struct st_node *slist_node;

//
// Constructors
//

// Creates a new slist reference.
slist_ref new_slist (void);

// Creates a new slist node containing string representations of 
// the information in the file stat st of fname.
slist_node new_slistnode (struct stat *st, char *fname);

//
// Accessors/Mutators
// 

// Inserts node into slist in ascending lexicographic order. The
// lowercase string representation of the filename contained in node
// is used.
void insert_slist (slist_ref slist, slist_node node);

// Generates a stack of slist nodes that are directories in slist. 
// They are pushed in ascending lexicographic order.
void stack_slistdir (slist_ref slist);
bool has_empty_dirstack (slist_ref slist);

// Returns the filename character string of the top node in the 
// directory stack in slist.
char *pop_slistdir (slist_ref slist);

// Frees slsit and all its nodes.
void free_slist (slist_ref slist);

// Frees a single slist node.
void free_slistnode (slist_node node);

// Prints all the information in the nodes in slist to stdout. 
// Formatting is similar to the command ls -l.
void print_slist_long (slist_ref slist);

// Prints only the filename of each node in slist to stdout. One 
// oer line.
void print_slist_short (slist_ref slist);

// True if node is a directory, otherwise false.
bool is_dir (slist_node node);

// True if node is a hidden file, otherwise false.
bool is_hidden (slist_node node);

#endif
