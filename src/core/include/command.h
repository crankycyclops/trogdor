
#ifndef COMMAND_H
#define COMMAND_H


#include <dstring.h>

/* All commands are parsed down to a required verb and an optional direct
   object and preposition + indirect object */
typedef struct command {
   dstring_t  verb;
   dstring_t  directObject;
   dstring_t  preposition;
   dstring_t  indirectObject;
   int        error; 
} Command;

#ifndef COMMAND_C

/* Executes a user input command. */
extern void executeCommand();

#endif


#endif

