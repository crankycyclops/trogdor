
#include <stdio.h>
#include "include/string.h"

#define MAX_COMMAND_LENGTH 250


char *readCommand() {

   /* declared static because we return a pointer to it */
   static char command[MAX_COMMAND_LENGTH] = "";

   printf("> ");
   fgets(command, MAX_COMMAND_LENGTH, stdin);

   // trim surrounding whitespace and convert to all lower case
   ltrim(command);
   rtrim(command);
   strtolower(command);

   return command;
}

