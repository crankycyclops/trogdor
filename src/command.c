
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "trogdor.h"
#include "state.h"
#include "shell.h"
#include "string.h"
#include "action.h"

/* executes a user input command */
void executeCommand();


void executeCommand() {

   char *command;

   command = readCommand();

   if (0 == strncmp(command, "north", MAX_COMMAND_LENGTH)) {
      move(NORTH);
   }

   else if (0 == strncmp(command, "south", MAX_COMMAND_LENGTH)) {
      move(SOUTH);
   }

   else if (0 == strncmp(command, "east", MAX_COMMAND_LENGTH)) {
      move(EAST);
   }

   else if (0 == strncmp(command, "west", MAX_COMMAND_LENGTH)) {
      move(WEST);
   }

   // TODO: add support for synonyms and add exit as synonym
   else if (0 == strncmp(command, "quit", MAX_COMMAND_LENGTH)) {
      printf("Goodbye!\n");
      exit(EXIT_SUCCESS);
   }

   else {
      printf("Sorry, I don't understand you.\n");
   }

   return;
}




