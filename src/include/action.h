
#ifndef ACTION_H
#define ACTION_H


typedef struct verb {
   char *word;              /* verb that maps to a specific action */
   int (*action)(Command);  /* pointer to the action we call for the verb */
} Verb;

#ifndef ACTION_C

#include "trogdor.h"
#include "command.h"

/* executes an action corresponding to the command's verb */
extern int callAction(Command command);

#endif


#endif

