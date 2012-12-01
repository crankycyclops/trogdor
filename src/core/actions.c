
#include <stdio.h>
#include <stdlib.h>
#include <dstring.h>

#include "include/trogdor.h"
#include "include/data.h"
#include "include/object.h"
#include "include/room.h"
#include "include/data.h"
#include "include/state.h"
#include "include/command.h"
#include "include/object.h"
#include "include/room.h"

#define OBJ_FROM_ROOM       1
#define OBJ_FROM_INVENTORY  2


/* psuedo action that frees allocated memory and quits the game */
int actionQuit(Command command);

/* look at objects or describe the room again */
int actionLook(Command command);

/* list the items in the user's inventory */
int actionList(Command command);

/* moves the user in the specified direction */
int actionMove(Command command);

/* allows the user to pick up an object (requires direct object) */
int actionPickupObject(Command command);

/* allows the user to drop an object */
int actionDropObject(Command command);

/* returns object referenced by name, and disambiguates between synonyms if 
   necessary */
static Object *getObject(dstring_t name, int objectSource);

/******************************************************************************/

int actionQuit(Command command) {

   destroyData();
   g_outputString("Goodbye!\n");
   exit(EXIT_SUCCESS);
}

/******************************************************************************/

int actionLook(Command command) {

   /* name of the object we want to look at */
   dstring_t object = NULL;

   if (NULL != command.directObject) {
      object = command.directObject;
   }

   if (NULL != command.indirectObject) {

      /* having both a direct and indirect object is a syntax error */
      if (NULL != object) {
         return 0;
      }

      object = command.indirectObject;
   }

   /* user can type "look" or "look at room"; both do the same thing */
   if (NULL != object && 0 == strcmp(dstrview(object), "room")) {
      object = NULL;
   }

   /* user is looking at an object */
   if (NULL != object) {

      Object *thing;

      /* objects in the room have precedence over objects in the inventory
         -- no good reason to do this, except that it makes the code easier :) */
      thing = getObject(object, OBJ_FROM_ROOM);

      if (NULL == thing) {
         thing = getObject(object, OBJ_FROM_INVENTORY);
      }

      if (NULL == thing) {
         g_outputString("There is no %s here!\n", dstrview(object));
      }

      else {
         displayObject(thing);
      }
   }

   /* user is looking at the current room */
   else {
      displayRoom(location, TRUE);
   }

   return 1;
}

/******************************************************************************/

int actionList(Command command) {

   int totalWeight = 0;
   GList *item;

   /* list should only be a one word command */
   if (NULL != command.directObject || NULL != command.indirectObject) {
      return 0;
   }

   if (inventory.list != NULL) {
      g_outputString("Items in your inventory:\n");
      for (item = inventory.list; item != NULL; item = item->next) {

         Object *object = (Object *)item->data;
         totalWeight += object->weight;

         g_outputString("%s", dstrview(object->name));

         /* if the inventory has a finite weight, list that data as well */
         if (inventory.maxWeight > 0) {

            if (0 == object->weight) {
               g_outputString(" (weighs nothing)");
            }

            else {
               g_outputString(" (%1.1f %%)",
                  100 * ((double)object->weight / (double)inventory.maxWeight));
            }
         }

         g_outputString("\n");
      }
   }

   else {
      g_outputString("You don't have anything!\n");
   }

   if (inventory.maxWeight > 0) {
      g_outputString("\nUsed: %d/%d\n", totalWeight,
         inventory.maxWeight);
   }

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
         g_outputString("Where would you like to go?\n");
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
            g_outputString("You can't go that way!\n");
         }

         return 1;
   }

   else if (0 == strcmp("south", dstrview(direction))) {

         if (NULL != location->south) {
            setLocation(location->south);
         }

         else {
            g_outputString("You can't go that way!\n");
         }

         return 1;
   }

   else if (0 == strcmp("east", dstrview(direction))) {

         if (NULL != location->east) {
            setLocation(location->east);
         }

         else {
            g_outputString("You can't go that way!\n");
         }

         return 1;
   }

   else if (0 == strcmp("west", dstrview(direction))) {

         if (NULL != location->west) {
            setLocation(location->west);
         }

         else {
            g_outputString("You can't go that way!\n");
         }

         return 1;
   }

   // syntax error: no such direction
   else {
      return 0;
   }
}

/******************************************************************************/

static Object *getObject(dstring_t name, int objectSource) {

   GList *objectsByName;
   GList *curObject;

   int listCount;

   switch (objectSource) {

      case OBJ_FROM_ROOM:
         objectsByName = g_hash_table_lookup(location->objectByName,
            dstrview(name));
         break;

      case OBJ_FROM_INVENTORY:
         objectsByName = g_hash_table_lookup(inventory.byName, dstrview(name));
         break;

      default:
         fprintf(stderr, "called getObject() with an invalid source!\n");
         exit(EXIT_FAILURE);
   }

   curObject = objectsByName;
   listCount = g_list_length(objectsByName);

   /* the object doesn't exist */
   if (NULL == objectsByName) {
      return NULL;
   }

   /* there's only one object, so there's no ambiguity */
   else if (1 == listCount) {
      return (Object *)objectsByName->data;
   }

   /* we have to disambiguate between multiple objects */
   else {
      Object *object = clarifyObject(objectsByName, listCount);
      return object;
   }   
}

/******************************************************************************/

int actionPickupObject(Command command) {

   Object *object;

   /* make sure the user actually specified what they want to take */
   if (NULL == command.directObject) {
      // TODO: add support for prompting the user for clarification
      // (e.g. "What would you like to take?")
      g_outputString("Tell me what you want to %s!\n", dstrview(command.verb));
      return 1;
   }

   object = getObject(command.directObject, OBJ_FROM_ROOM);

   if (NULL == object) {
      g_outputString("There is no %s here!\n", dstrview(command.directObject));
   }

   else {
      takeObject(object);
   }

   // always return 1 only because so far, there are no possible syntax errors
   return 1;
}

/******************************************************************************/

int actionDropObject(Command command) {

   Object *object;

   /* make sure the user actually specified what they want to drop */
   if (NULL == command.directObject) {
      // TODO: add support for prompting the user for clarification
      // (e.g. "What would you like to drop?")
      g_outputString("Tell me what you want to %s!\n", dstrview(command.verb));
      return 1;
   }

   object = getObject(command.directObject, OBJ_FROM_INVENTORY);

   if (NULL == object) {
      // TODO: select a/an depending on vowel rules to make it prettier
      g_outputString("You don't have a %s!\n", dstrview(command.directObject));
   }

   else {
      dropObject(object);
   }

   // always return 1 only because so far, there are no possible syntax errors
   return 1;
}

