
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "trogdor.h"
#include "state.h"
#include "shell.h"
#include "string.h"
#include "action.h"

/* All commands are parsed down to a required verb and an optional direct
   object and preposition + indirect object */
typedef struct command {
   char *verb;
   char *directObject;
   char *preposition;
   char *indirectObject;
} Command;

/* executes a user input command */
void executeCommand();

/* main parse function that breaks a sentence down into its command parts */
static Command parseCommand(char *sentence);


void executeCommand() {

   Command command;

   command = parseCommand(readCommand());

   if (0 == strncmp(command.verb, "north", MAX_COMMAND_LENGTH)) {
      move(NORTH);
   }

   else if (0 == strncmp(command.verb, "south", MAX_COMMAND_LENGTH)) {
      move(SOUTH);
   }

   else if (0 == strncmp(command.verb, "east", MAX_COMMAND_LENGTH)) {
      move(EAST);
   }

   else if (0 == strncmp(command.verb, "west", MAX_COMMAND_LENGTH)) {
      move(WEST);
   }

   // TODO: add support for synonyms and add exit as synonym
   else if (0 == strncmp(command.verb, "quit", MAX_COMMAND_LENGTH)) {
      printf("Goodbye!\n");
      exit(EXIT_SUCCESS);
   }

   else {
      printf("Sorry, I don't understand you.\n");
   }

   return;
}


static Command parseCommand(char *sentence) {

   Command command = {NULL, NULL, NULL, NULL};

   // TODO
   command.verb = sentence;
   return command;
}

