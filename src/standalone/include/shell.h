
#ifndef SHELL_H
#define SHELL_H


#include <dstring.h>

/* Returns the command read by the shell, always in all lowercase letters. */
extern dstring_t readCommandFromStdin();


#endif

