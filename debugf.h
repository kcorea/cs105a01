//
// DESCRIPTION
//    Debugging library
//

#ifndef __DEBUGF_H__
#define __DEBUGF_H__


// Tell debugf what program is running.
void set_execname (char *name);

// Support for stub messages.
#define STUBPRINTF(...) \
        __stubprintf (__FILE__, __LINE__, __func__, __VA_ARGS__)
void __stubprintf (char *file, int line, const char *func,
                   char *format, ...);

// Debugging utility.
void set_debugflags (char *flags);

#define DEBUGF(FLAG,...) \
        __debugprintf (FLAG, __FILE__, __LINE__, __VA_ARGS__)
void __debugprintf (char flag, char *file, int line,
                    char *format, ...);

#endif
