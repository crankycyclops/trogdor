
#include <stdio.h>
#include <stdlib.h>

#include "include/trogdor.h"
#include "include/data.h"
#include "include/state.h"
#include "include/command.h"
#include "include/utility.h"
#include "include/shell.h"


/* psuedo action that frees allocated memory and quits the game */
int quitGame(Command command);

/* moves the user in the specified direction */
int move(Command command);

/* allows the user to pick up an object (requires direct object) */
int pickupObject(Command command);

/* processes the posession of an object from the current room */
static void takeObject(Object *object);

/* disambiguates in the case where a name refers to more than one object */
static Object *clarifyObject(GList *objects, int objectCount);

/******************************************************************************/

int quitGame(Command command) {

   destroyData();
   printf("Goodbye!\n");
   exit(EXIT_SUCCESS);
}

/******************************************************************************/

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


/******************************************************************************/

int pickupObject(Command command) {

   GList *objectsByName;
   GList *curObject;

   int listCount;

   /* make sure the user actually specified what they want to take */
   if (NULL == command.directObject) {
      // TODO: add support for prompting the user for clarification
      // (e.g. "What would you like to take?")
      printf("Tell me what you want to %s!\n", dstrview(command.verb));
      return 1;
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
      printf("You take the %s.\n", dstrview(command.directObject));
   }

   /* we have to disambiguate between multiple objects */
   else {
      Object *object = clarifyObject(objectsByName, listCount);
      takeObject(object);
      printf("You take the %s.\n", dstrview(object->name));
      return 1;
   }
}

/******************************************************************************/

static void takeObject(Object *object) {

   // TODO
   printf("STUB: pretending to take object \"%s\" ;)\n", dstrview(object->name));
   return;
}

/******************************************************************************/

static Object *clarifyObject(GList *objects, int objectCount) {

   int        i;
   int        objectFound;
   dstring_t  name;

   Object     *object;
   GList      *curObject = objects;

   printf ("Do you mean the ");

   for (i = 0; curObject != NULL; i++) {

      printf("%s", dstrview(((Object *)curObject->data)->name));

      if (i < objectCount - 2) {
         printf(", ");
      }

      else if (i < objectCount - 1) {
         printf(" or the ");
      }

      curObject = g_list_next(curObject);
   }

   printf("?\n");

   /* get an object name from the shell */
   do {
      name = readCommand();
   } while (0 == dstrlen(name));

   /* locate the correct object */
   curObject = objects;
   object = NULL;
   objectFound = 0;

   while (curObject != NULL) {

      object = (Object *)curObject->data;

      if (0 == strcmp(dstrview(object->name), dstrview(name))) {
         objectFound = 1;
         break;
      }

      curObject = g_list_next(curObject);
   }

   /* if user entered a non-existent value, try again until we succeed */
   if (!objectFound) {
      printf("There is no %s here!\n", dstrview(name));
      return clarifyObject(objects, objectCount);
   }

   return object;
}

