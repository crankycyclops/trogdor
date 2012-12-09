
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#include "include/trogdor.h"
#include "include/data.h"
#include "include/data.h"
#include "include/room.h"
#include "include/object.h"
#include "include/state.h"
#include "include/player.h"
#include "include/command.h"
#include "include/object.h"
#include "include/room.h"

#define OBJ_FROM_ROOM       1
#define OBJ_FROM_INVENTORY  2


/* psuedo action that frees allocated memory and quits the game */
int actionQuit(Player *player, Command command);

/* look at objects or describe the room again */
int actionLook(Player *player, Command command);

/* list the items in the user's inventory */
int actionList(Player *player, Command command);

/* moves the user in the specified direction */
int actionMove(Player *player, Command command);

/* allows the user to pick up an object (requires direct object) */
int actionPickupObject(Player *player, Command command);

/* allows the user to drop an object */
int actionDropObject(Player *player, Command command);

/* allows the user to jump */
// TODO: support jumping to places, triggering jump events, etc?
int actionJump(Player *player, Command command);

/* responds to profanity */
int actionProfanity(Player *player, Command command);

/* returns object referenced by name, and disambiguates between synonyms if 
   necessary */
static Object *getObject(Player *player, dstring_t name, int objectSource);

/******************************************************************************/

int actionQuit(Player *player, Command command) {

   destroyGame();
}

/******************************************************************************/

int actionLook(Player *player, Command command) {

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
      thing = getObject(player, object, OBJ_FROM_ROOM);

      if (NULL == thing) {
         thing = getObject(player, object, OBJ_FROM_INVENTORY);
      }

      if (NULL == thing) {
         g_outputString("There is no %s here!\n", dstrview(object));
      }

      else {
         displayObject(player, thing, TRUE);
      }
   }

   /* user is looking at the current room */
   else {
      displayRoom(player, player->location, TRUE);
   }

   return 1;
}

/******************************************************************************/

int actionList(Player *player, Command command) {

   int totalWeight = 0;
   GList *item;

   /* list should only be a one word command */
   if (NULL != command.directObject || NULL != command.indirectObject) {
      return 0;
   }

   if (player->inventory.list != NULL) {
      g_outputString("Items in your inventory:\n");
      for (item = player->inventory.list; item != NULL; item = item->next) {

         Object *object = (Object *)item->data;
         totalWeight += object->weight;

         g_outputString("%s", dstrview(object->name));

         /* if the inventory has a finite weight, list that data as well */
         if (player->inventory.maxWeight > 0) {

            if (0 == object->weight) {
               g_outputString(" (weighs nothing)");
            }

            else {
               g_outputString(" (%1.1f %%)",
                  100 * ((double)object->weight /
                  (double)player->inventory.maxWeight));
            }
         }

         g_outputString("\n");
      }
   }

   else {
      g_outputString("You don't have anything!\n");
   }

   if (player->inventory.maxWeight > 0) {
      g_outputString("\nUsed: %d/%d\n", totalWeight,
         player->inventory.maxWeight);
   }

   return 1;
}

/******************************************************************************/

int actionMove(Player *player, Command command) {

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

         if (NULL != player->location->north) {
            setLocation(player, player->location->north, 1);
         }

         else {
            g_outputString("You can't go that way!\n");
         }

         return 1;
   }

   else if (0 == strcmp("south", dstrview(direction))) {

         if (NULL != player->location->south) {
            setLocation(player, player->location->south, 1);
         }

         else {
            g_outputString("You can't go that way!\n");
         }

         return 1;
   }

   else if (0 == strcmp("east", dstrview(direction))) {

         if (NULL != player->location->east) {
            setLocation(player, player->location->east, 1);
         }

         else {
            g_outputString("You can't go that way!\n");
         }

         return 1;
   }

   else if (0 == strcmp("west", dstrview(direction))) {

         if (NULL != player->location->west) {
            setLocation(player, player->location->west, 1);
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

static Object *getObject(Player *player, dstring_t name, int objectSource) {

   GList *objectsByName;
   GList *curObject;

   int listCount;

   switch (objectSource) {

      case OBJ_FROM_ROOM:
         objectsByName = g_hash_table_lookup(player->location->objectByName,
            dstrview(name));
         break;

      case OBJ_FROM_INVENTORY:
         objectsByName = g_hash_table_lookup(player->inventory.byName,
            dstrview(name));
         break;

      default:
         g_outputError("called getObject() with an invalid source!\n");
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

int actionPickupObject(Player *player, Command command) {

   Object *object;

   /* make sure the user actually specified what they want to take */
   if (NULL == command.directObject) {
      // TODO: add support for prompting the user for clarification
      // (e.g. "What would you like to take?")
      g_outputString("Tell me what you want to %s!\n", dstrview(command.verb));
      return 1;
   }

   object = getObject(player, command.directObject, OBJ_FROM_ROOM);

   if (NULL == object) {
      g_outputString("There is no %s here!\n", dstrview(command.directObject));
   }

   else {
      takeObject(player, object);
   }

   // always return 1 only because so far, there are no possible syntax errors
   return 1;
}

/******************************************************************************/

int actionDropObject(Player *player, Command command) {

   Object *object;

   /* make sure the user actually specified what they want to drop */
   if (NULL == command.directObject) {
      // TODO: add support for prompting the user for clarification
      // (e.g. "What would you like to drop?")
      g_outputString("Tell me what you want to %s!\n", dstrview(command.verb));
      return 1;
   }

   object = getObject(player, command.directObject, OBJ_FROM_INVENTORY);

   if (NULL == object) {
      // TODO: select a/an depending on vowel rules to make it prettier
      g_outputString("You don't have a %s!\n", dstrview(command.directObject));
   }

   else {
      dropObject(player, object);
   }

   // always return 1 only because so far, there are no possible syntax errors
   return 1;
}

/******************************************************************************/

// TODO: support jumping to places, triggering jump events, etc?
int actionJump(Player *player, Command command) {

   if (command.directObject != NULL || command.indirectObject != NULL) {
      return 0;
   }

   g_outputString("Weeee!\n");
   return 1;
}

int test(Player *player, Command *command) {

   return 1;
}

/******************************************************************************/

int actionProfanity(Player *player, Command command) {

   static char *responses[] = {
      "Such language!\n",
      "You, sir, have a foul mouth!\n",
      "Well, ?&*@! to you too!\n",
      "Do you miss your mother with that mouth?\n",
      "Classy.\n"
   };

   static int arrSize = sizeof(responses) / sizeof (char *);

   int i = (rand() % arrSize) - 1;

   if (i < 0) {
      i = 0;
   }

   srand(time(NULL));
   g_outputString("%s", responses[i]);

   return 1;
}

