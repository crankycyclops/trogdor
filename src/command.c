
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dstring.h>

#include "include/trogdor.h"
#include "include/state.h"
#include "include/shell.h"
#include "include/token.h"
#include "include/action.h"

/* All commands are parsed down to a required verb and an optional direct
   object and preposition + indirect object */
typedef struct command {
   dstring_t verb;
   dstring_t directObject;
   dstring_t preposition;
   dstring_t indirectObject;
} Command;

/* executes a user input command */
void executeCommand();

/* main parse function that breaks a sentence down into its command parts */
static Command parseCommand(dstring_t sentence);

/* initializes the dstring_t objects inside of the command struct */
static void initCommand(Command *command);

/* frees memory used by the command object */
static void destroyCommand(Command *command);


static void initCommand(Command *command) {

      command->verb = NULL;
      command->directObject = NULL;
      command->preposition = NULL;
      command->indirectObject = NULL;

      if (DSTR_SUCCESS != dstralloc(&command->verb)) {
         fprintf(stderr, "out of memory\n");
         exit(EXIT_FAILURE);
      }

      if (DSTR_SUCCESS != dstralloc(&command->directObject)) {
         fprintf(stderr, "out of memory\n");
         exit(EXIT_FAILURE);
      }

      if (DSTR_SUCCESS != dstralloc(&command->preposition)) {
         fprintf(stderr, "out of memory\n");
         exit(EXIT_FAILURE);
      }

      if (DSTR_SUCCESS != dstralloc(&command->indirectObject)) {
         fprintf(stderr, "out of memory\n");
         exit(EXIT_FAILURE);
      }

      return;
}


static void destroyCommand(Command *command) {

      if (DSTR_SUCCESS != dstrfree(&command->verb)) {
         fprintf(stderr, "error freeing memory\n");
         exit(EXIT_FAILURE);
      }

      if (DSTR_SUCCESS != dstrfree(&command->directObject)) {
         fprintf(stderr, "error freeing memory\n");
         exit(EXIT_FAILURE);
      }

      if (DSTR_SUCCESS != dstrfree(&command->preposition)) {
         fprintf(stderr, "error freeing memory\n");
         exit(EXIT_FAILURE);
      }

      if (DSTR_SUCCESS != dstrfree(&command->indirectObject)) {
         fprintf(stderr, "error freeing memory\n");
         exit(EXIT_FAILURE);
      }

      return;
}


void executeCommand() {

   Command command;

   command = parseCommand(readCommand());

   if (0 == strcmp(dstrview(command.verb), "north")) {
      move(NORTH);
   }

   else if (0 == strcmp(dstrview(command.verb), "south")) {
      move(SOUTH);
   }

   else if (0 == strcmp(dstrview(command.verb), "east")) {
      move(EAST);
   }

   else if (0 == strcmp(dstrview(command.verb), "west")) {
      move(WEST);
   }

   // TODO: add support for synonyms and add exit as synonym
   else if (0 == strcmp(dstrview(command.verb), "quit")) {
      printf("Goodbye!\n");
      exit(EXIT_SUCCESS);
   }

   else {
      printf("Sorry, I don't understand you.\n");
   }

   // TODO: this causes segfault the 2nd time; debug this!
   destroyCommand(&command);
   return;
}


static Command parseCommand(dstring_t sentence) {

   Command command;
   initCommand(&command);

   // TODO
   command.verb = sentence;

   dstrfree(&sentence);
   return command;
}

