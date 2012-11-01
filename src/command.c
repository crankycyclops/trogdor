
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "trogdor.h"
#include "shell.h"

#define NORTH 1
#define SOUTH 2
#define EAST  3
#define WEST  4

#include "string.h"

/* executes a user input command */
void executeCommand();

/* moves the user in the specified direction */
static void move(int direction);


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
      printf("Sorry, I don't understand you.");
   }

   return;
}


static void move(int direction) {

   switch (direction) {

      case NORTH:
         printf("NORTH!\n");
         break;

      case SOUTH:
         printf("SOUTH!\n");
         break;

      case EAST:
         printf("EAST!\n");
         break;

      case WEST:
         printf("WEST!\n");
         break;

      default:
         break;
   }

   return;
}

