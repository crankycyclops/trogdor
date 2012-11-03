
#include <stdio.h>
#include <stdlib.h>

#include "include/trogdor.h"
#include "include/state.h"
#include "include/data.h"
#include "include/command.h"

typedef struct verb {
   char *word;              /* verb that maps to a specific action */
   int (*action)(Command);  /* pointer to the action we call for the verb */
} Verb;

/*
   Calls whatever action corresponds to the given command.

   Output:
      If there was a syntax error, or if the given verb doesn't correspond to
      any known action, false.  Otherwise, true.
*/
int callAction(Command command);

/* moves the user in the specified direction */
int move(Command command);

/* psuedo action that frees allocated memory and quits the game */
int quitGame(Command command);


int callAction(Command command) {

   int i;

   static Verb verbs[] = {
      {"north", &move},
      {"south", &move},
      {"east",  &move},
      {"west",  &move},
      {"quit",  &quitGame},
      {NULL, NULL}
   };

   for (i = 0; verbs[i].word != NULL; i++) {
      if (0 == strcmp(verbs[i].word, dstrview(command.verb))) {
         return verbs[i].action(command);
      }
   }

   /* action was not found, so the verb must not exist in our vocabulary */
   return 0;
}


int quitGame(Command command) {

   destroyData();
   printf("Goodbye!\n");
   exit(EXIT_SUCCESS);
}


int move(Command command) {

   if (0 == strcmp("north", dstrview(command.verb))) {

         if (NULL != location->north) {
            location = location->north;
            printf("%s\n", location->description);
         }

         else {
            printf("You can't go that way!\n");
         }

         return 1;
   }

   else if (0 == strcmp("south", dstrview(command.verb))) {

         if (NULL != location->south) {
            location = location->south;
            printf("%s\n", location->description);
         }

         else {
            printf("You can't go that way!\n");
         }

         return 1;
   }

   else if (0 == strcmp("east", dstrview(command.verb))) {

         if (NULL != location->east) {
            location = location->east;
            printf("%s\n", location->description);
         }

         else {
            printf("You can't go that way!\n");
         }

         return 1;
   }

   else if (0 == strcmp("west", dstrview(command.verb))) {

         if (NULL != location->west) {
            location = location->west;
            printf("%s\n", location->description);
         }

         else {
            printf("You can't go that way!\n");
         }

         return 1;
   }

   // syntax error: no such direction
   else {
      return 0;
   }
}

