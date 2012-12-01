
#ifndef SHELL_H
#define SHELL_H


#include <dstring.h>

/* Calls printf to output a string to stdout */
extern int consoleOut(const char *format, ...);

/* Returns the command read by the shell, always in all lowercase letters. */
extern dstring_t consoleIn();


#endif

