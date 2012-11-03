
#include <stdio.h>
#include <stdlib.h>

#include "include/trogdor.h"
#include "include/data.h"
#include "include/state.h"
#include "include/command.h"

/* moves the user in the specified direction */
int move(Command command);

/* psuedo action that frees allocated memory and quits the game */
int quitGame(Command command);


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

