
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#include "include/trogdor.h"
#include "include/data.h"
#include "include/room.h"
#include "include/object.h"
#include "include/state.h"
#include "include/player.h"
#include "include/command.h"
#include "include/object.h"
#include "include/creature.h"
#include "include/room.h"
#include "include/combat.h"

#define OBJ_FROM_ROOM       1
#define OBJ_FROM_INVENTORY  2


/* psuedo action that frees allocated memory and quits the game */
int actionQuit(Player *player, Command command);

/* look at objects or describe the room again */
int actionLook(Player *player, Command command);

/* list the items in the user's inventory */
int actionList(Player *player, Command command);

/* display's information about a player's health */
int actionHealth(Player *player, Command command);

/* moves the user in the specified direction */
int actionMove(Player *player, Command command);

/* allows the user to pick up an object (requires direct object) */
int actionPickupObject(Player *player, Command command);

/* allows the user to drop an object */
int actionDropObject(Player *player, Command command);

/* allows a player to attack a creature or another player */
int actionAttack(Player *player, Command command);

/* allows the user to jump */
// TODO: support jumping to places, triggering jump events, etc?
int actionJump(Player *player, Command command);

/* responds to profanity */
int actionProfanity(Player *player, Command command);

/* returns creature referenced by the given name */
static Creature *getCreature(Player *player, dstring_t name);

/* returns object referenced by the name, and disambiguates between synonyms if 
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

   /* user is looking at an object or a creature */
   if (NULL != object) {

      Creature *being = NULL;
      Object   *thing = NULL;

      /* objects in the room have precedence over objects in the inventory
         -- no good reason to do this, except that it makes the code easier :) */
      thing = getObject(player, object, OBJ_FROM_ROOM);

      if (NULL == thing) {
         thing = getObject(player, object, OBJ_FROM_INVENTORY);
      }

      /* couldn't find the referenced item in the room or inventory - is it a
         creature? */
      if (NULL == thing) {
         being = getCreature(player, object);
      }

      /* objects take precedence over creatures */
      if (NULL != thing) {
         displayObject(player, thing, TRUE);
      }

      else if (NULL != being) {
         displayCreature(player, being, TRUE);
      }

      else {
         g_outputString("There is no %s here!\n", dstrview(object));
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

int actionHealth(Player *player, Command command) {

   /* health should only be a one word command */
   if (NULL != command.directObject || NULL != command.indirectObject) {
      return 0;
   }

   if (player->maxHealth > 0) {

      g_outputString("%d/%d\n", player->state.health,
         player->maxHealth);

      if (!player->state.alive) {
         g_outputString("You are dead.\n");
      }
   }

   else {
      g_outputString("You are immortal.\n");
   }
}

/******************************************************************************/

int actionMove(Player *player, Command command) {

   Room       *goHere;
   dstring_t  direction;

   /* transition messages (if defined) */
   dstring_t  transitionIn;
   dstring_t  transitionOut;

   /* user entered navigation command in the verb + direction format */
   if (
      0 != strcmp("north", dstrview(command.verb)) &&
      0 != strcmp("south", dstrview(command.verb)) &&
      0 != strcmp("east",  dstrview(command.verb)) &&
      0 != strcmp("west",  dstrview(command.verb)) &&
      0 != strcmp("in",    dstrview(command.verb)) &&
      0 != strcmp("out",   dstrview(command.verb)) &&
      0 != strcmp("up",    dstrview(command.verb)) &&
      0 != strcmp("down",  dstrview(command.verb))
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
      transitionOut = getMessage(player->location->messages, "goNorth");
      transitionIn  = NULL != player->location->north ?
         getMessage(player->location->north->messages, "fromSouth") : NULL;
      goHere = player->location->north;
   }

   else if (0 == strcmp("south", dstrview(direction))) {
      transitionOut = getMessage(player->location->messages, "goSouth");
      transitionIn  = NULL != player->location->south ?
         getMessage(player->location->south->messages, "fromNorth") : NULL;
      goHere = player->location->south;
   }

   else if (0 == strcmp("east", dstrview(direction))) {
      transitionOut = getMessage(player->location->messages, "goEast");
      transitionIn  = NULL != player->location->east ?
         getMessage(player->location->east->messages, "fromWest") : NULL;
      goHere = player->location->east;
   }

   else if (0 == strcmp("west", dstrview(direction))) {
      transitionOut = getMessage(player->location->messages, "goWest");
      transitionIn  = NULL != player->location->west ?
         getMessage(player->location->west->messages, "fromEast") : NULL;
      goHere = player->location->west;
   }

   else if (0 == strcmp("in", dstrview(direction))) {
      transitionOut = getMessage(player->location->messages, "goIn");
      transitionIn  = NULL != player->location->in ?
         getMessage(player->location->in->messages, "fromOut") : NULL;
      goHere = player->location->in;
   }

   else if (0 == strcmp("out", dstrview(direction))) {
      transitionOut = getMessage(player->location->messages, "goOut");
      transitionIn  = NULL != player->location->out ?
         getMessage(player->location->out->messages, "fromIn") : NULL;
      goHere = player->location->out;
   }

   else if (0 == strcmp("up", dstrview(direction))) {
      transitionOut = getMessage(player->location->messages, "goUp");
      transitionIn  = NULL != player->location->up ?
         getMessage(player->location->up->messages, "fromDown") : NULL;
      goHere = player->location->up;
   }

   else if (0 == strcmp("down", dstrview(direction))) {
      transitionOut = getMessage(player->location->messages, "goDown");
      transitionIn  = NULL != player->location->down ?
         getMessage(player->location->down->messages, "fromUp") : NULL;
      goHere = player->location->down;
   }

   /* syntax error: no such direction */
   else {
      return 0;
   }

   if (NULL == goHere) {
      g_outputString("You can't go that way!\n");
   }

   else {

      if (NULL != transitionOut) {
         g_outputString("\n%s\n", dstrview(transitionOut));
      }

      if (NULL != transitionIn) {
         g_outputString("\n%s\n", dstrview(transitionIn));
      }

      setLocation(player, goHere, 1);
   }

   return 1;
}

/******************************************************************************/

static Creature *getCreature(Player *player, dstring_t name) {

   GList *matches;
   GList *curMatch;

   matches = g_hash_table_lookup(player->location->creatureByName,
      dstrview(name));

   // TODO: clarify between synonyms (right now, there aren't any...)
   return NULL == matches ? NULL : (Creature *)matches->data;
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

// TODO: add support for attacking players as well as creatures
int actionAttack(Player *player, Command command) {

   Creature *target = NULL;
   Object   *weapon = NULL;

   // TODO: add support for generic synonyms such as "creature" or "player"

   // player didn't specify name of entity to attack
   if (NULL == command.directObject) {
      // TODO: add support for attacking whatever creature is in room
      return 0;
   }

   if (NULL != command.indirectObject) {

      weapon = getObject(player, command.indirectObject, OBJ_FROM_INVENTORY);

      if (NULL == weapon) {
         g_outputString("You don't have a %s!\n", dstrview(command.indirectObject));
         return 1;
      }

      else if (!weapon->weapon) {
         g_outputString("A %s is not a weapon!\n", dstrview(command.indirectObject));
         return 1;
      }
   }

   target = getCreature(player, command.directObject);

   if (NULL == target) {
      g_outputString("There is no %s to attack!\n", dstrview(command.directObject));
   }

   else if (!target->attackable) {
      g_outputString("You cannot attack %s!\n", dstrview(command.directObject));
   }

   else {
      attack(player, entity_player, target, entity_creature, weapon, TRUE);
   }

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

/******************************************************************************/

int actionProfanity(Player *player, Command command) {

   static char *responses[] = {
      "Such language!\n",
      "You, sir, have a foul mouth!\n",
      "Well, ?&*@! to you too!\n",
      "Do you kiss your mother with that mouth?\n",
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

