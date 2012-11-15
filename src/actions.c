
#include <stdio.h>
#include <stdlib.h>
#include <dstring.h>

#include "include/trogdor.h"
#include "include/data.h"
#include "include/state.h"
#include "include/command.h"
#include "include/shell.h"
#include "include/object.h"
#include "include/room.h"


/* psuedo action that frees allocated memory and quits the game */
int actionQuit(Command command);

/* look at objects or describe the room again */
int actionLook(Command command);

/* moves the user in the specified direction */
int actionMove(Command command);

/* allows the user to pick up an object (requires direct object) */
int actionPickupObject(Command command);

/* allows the user to drop an object */
int actionDropObject(Command command);

/******************************************************************************/

int actionQuit(Command command) {

   destroyData();
   printf("Goodbye!\n");
   exit(EXIT_SUCCESS);
}

/******************************************************************************/

int actionLook(Command command) {

   // TODO: add support for looking at things other than the room
   displayRoom(location);
   return 1;
}

/******************************************************************************/

int actionMove(Command command) {

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
            setLocation(location->north);
         }

         else {
            printf("You can't go that way!\n");
         }

         return 1;
   }

   else if (0 == strcmp("south", dstrview(direction))) {

         if (NULL != location->south) {
            setLocation(location->south);
         }

         else {
            printf("You can't go that way!\n");
         }

         return 1;
   }

   else if (0 == strcmp("east", dstrview(direction))) {

         if (NULL != location->east) {
            setLocation(location->east);
         }

         else {
            printf("You can't go that way!\n");
         }

         return 1;
   }

   else if (0 == strcmp("west", dstrview(direction))) {

         if (NULL != location->west) {
            setLocation(location->west);
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


/******************************************************************************/

int actionPickupObject(Command command) {

   GList *objectsByName;
   GList *curObject;

   int listCount;

   /* make sure the user actually specified what they want to take */
   if (NULL == command.directObject) {
      // TODO: add support for prompting the user for clarification
      // (e.g. "What would you like to take?")
      printf("Tell me what you want to %s!\n", dstrview(command.verb));
   }

   objectsByName = g_hash_table_lookup(location->objectByName,
      dstrview(command.directObject));
   curObject = objectsByName;
   listCount = g_list_length(objectsByName);

   /* the object doesn't exist */
   if (NULL == objectsByName) {
      printf("There is no %s here!\n", dstrview(command.directObject));
   }

   /* there's only one object, so there's no ambiguity */
   else if (1 == listCount) {
      takeObject((Object *)objectsByName->data);
   }

   /* we have to disambiguate between multiple objects */
   else {
      Object *object = clarifyObject(objectsByName, listCount);
      takeObject(object);
   }

   // always return 1 only because so far, there are no possible syntax errors
   return 1;
}

/******************************************************************************/

int actionDropObject(Command command) {

   // TODO
   printf("STUB: drop object\n");
   return 1;
}

