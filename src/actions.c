
#include <stdio.h>
#include <stdlib.h>

#include "include/trogdor.h"
#include "include/data.h"
#include "include/state.h"
#include "include/command.h"
#include "include/utility.h"


/* moves the user in the specified direction */
int move(Command command);

/* allows the user to pick up an object (requires direct object) */
int pickupObject(Command command);

/* claims an object for the user */
static int claimObject(dstring_t name);

/* psuedo action that frees allocated memory and quits the game */
int quitGame(Command command);


int quitGame(Command command) {

   destroyData();
   printf("Goodbye!\n");
   exit(EXIT_SUCCESS);
}


int pickupObject(Command command) {

   if (NULL == command.directObject) {
      // TODO: add support for prompting the user for clarification
      // (e.g. "What would you like to take?")
      printf("Tell me what you want to %s!\n", dstrview(command.verb));
      return 1;
   }

   if (claimObject(command.directObject)) {
      printf("You take the %s.\n", dstrview(command.directObject));
   }

   else {
      printf("There is no %s here!\n", dstrview(command.directObject));
   }

   return 1;
}


// TODO: other return codes if object exists but is "untakeable," etc.
static int claimObject(dstring_t name) {

   GList *objectsByName = g_hash_table_lookup(location->objectByName,
      dstrview(name));

   /* the object(s) exists */
   if (objectsByName) {
      // TODO: disambiguate synonyms
      // TODO: remove from hash table value's list and from room's list
      // TODO: add to inventory's hash table's list and to inventory list
      return 1;
   }

   /* the object does not exist */
   return 0;
}


int move(Command command) {

   dstring_t direction;

   /* user entered navigation command in the verb + direction format */
   if (
      0 != strcmp("north", dstrview(command.verb)) &&
      0 != strcmp("south", dstrview(command.verb)) &&
      0 != strcmp("east",  dstrview(command.verb)) &&
      0 != strcmp("west",  dstrview(command.verb))
   ) {

      if (NULL != command.directObject) {
         direction = command.directObject;
      }

      else if (NULL != command.indirectObject) {
         direction = command.indirectObject;
      }

      else {
         printf("Where would you like to go?\n");
         return 1;
      }
   }

   /* command was given as just a simple direction (e.g. "north") */
   else {
      direction = command.verb;
   }

   if (0 == strcmp("north", dstrview(direction))) {

         if (NULL != location->north) {
            location = location->north;
            displayRoom(location);
         }

         else {
            printf("You can't go that way!\n");
         }

         return 1;
   }

   else if (0 == strcmp("south", dstrview(direction))) {

         if (NULL != location->south) {
            location = location->south;
            displayRoom(location);
         }

         else {
            printf("You can't go that way!\n");
         }

         return 1;
   }

   else if (0 == strcmp("east", dstrview(direction))) {

         if (NULL != location->east) {
            location = location->east;
            displayRoom(location);
         }

         else {
            printf("You can't go that way!\n");
         }

         return 1;
   }

   else if (0 == strcmp("west", dstrview(direction))) {

         if (NULL != location->west) {
            location = location->west;
            displayRoom(location);
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

