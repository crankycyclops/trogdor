
#define CREATURE_C

#include <stdio.h>
#include <stdlib.h>

#include "include/trogdor.h"
#include "include/object.h"
#include "include/room.h"
#include "include/creature.h"
#include "include/data.h"
#include "include/state.h"
#include "include/command.h"
#include "include/event.h"
#include "include/messages.h"
#include "include/attributes.h"
#include "include/health.h"
#include "include/combat.h"


/* Allocates memory for a creature.  If initMessages is true, we allocate memory
   for the messages structure.  Otherwise, just set it to NULL. */
Creature *createCreature(int initMessages);

/* frees all memory allocated for a creature */
void destroyCreature(Creature *creature);

/* displays a creature (long or short) */
void displayCreature(Player *player, Creature *creature,
int showLongDescription);

/* describes a creature in detail */
static void describeCreature(Creature *creature);

/******************************************************************************/

Creature *createCreature(int initMessages) {

   Creature *newcreature;

   newcreature = malloc(sizeof(Creature));
   if (NULL == newcreature) {
      PRINT_OUT_OF_MEMORY_ERROR;
   }

   newcreature->L = NULL;
   newcreature->maxHealth = DEFAULT_CREATURE_MAXHEALTH;

   /* creature attributes */
   newcreature->attributes.strength = DEFAULT_CREATURE_STRENGTH;
   newcreature->attributes.dexterity = DEFAULT_CREATURE_DEXTERITY;
   newcreature->attributes.intelligence = DEFAULT_CREATURE_INTELLIGENCE;

   /* initialize the creature's state */
   newcreature->state.seenByPlayers = g_hash_table_new(g_str_hash, g_str_equal);
   newcreature->state.alive = DEFAULT_CREATURE_ALIVE;
   newcreature->state.health = DEFAULT_CREATURE_HEALTH;
   newcreature->state.seenByAPlayer = 0;

   /* initialize creature's inventory */
   newcreature->inventory.byName = g_hash_table_new(g_str_hash, g_str_equal);
   newcreature->inventory.list = NULL;

   newcreature->allegiance = CREATURE_ALLEGIANCE_NEUTRAL;
   newcreature->attackable = DEFAULT_CREATURE_ATTACKABLE;
   newcreature->woundRate = DEFAULT_CREATURE_WOUNDRATE;
   newcreature->counterattack = DEFAULT_CREATURE_COUNTERATTACK;

   newcreature->messages = NULL;
   if (initMessages) {
      newcreature->messages = createMessages();
   }

   return newcreature;
}

/******************************************************************************/

void destroyCreature(Creature *creature) {

   dstrfree(&creature->name);
   dstrfree(&creature->title);
   dstrfree(&creature->description);

   g_hash_table_destroy(creature->inventory.byName);
   g_list_free(creature->inventory.list);

   g_hash_table_destroy(creature->state.seenByPlayers);

   if (creature->L != NULL) {
      lua_close(creature->L);
   }

   if (NULL != creature->messages) {
      destroyMessages(creature->messages);
   }

   free(creature);
   return;
}

/******************************************************************************/

void displayCreature(Player *player, Creature *creature,
int showLongDescription) {

   if (!event("beforeDisplayCreature", 2, eventArgPlayer(player),
   eventArgCreature(creature))) {
      return;
   }

   if (TRUE == showLongDescription ||
   NULL == g_hash_table_lookup(creature->state.seenByPlayers,
   (char *)dstrview(player->name))) {

      creature->state.seenByAPlayer = 1;

      g_hash_table_insert(creature->state.seenByPlayers,
         (char *)dstrview(player->name), player);

      describeCreature(creature);
   }

   else {

      if (!creature->state.alive) {
         g_outputString("\nYou see the body of %s.\n",
            dstrview(creature->title));
      }

      else {
         g_outputString("\nYou see %s.\n",
            dstrview(creature->title));
      }
   }

   event("afterDisplayCreature", 2, eventArgPlayer(player),
      eventArgCreature(creature));
}

/******************************************************************************/

static void describeCreature(Creature *creature) {

   const char *description;

   /* creature is alive */
   if (creature->state.alive) {
      description = dstrview(creature->description);
   }

   /* creature is dead */
   else {

      dstring_t message = getMessage(creature->messages, "dead");

      if (NULL == message) {
         description = "It lays crumpled and lifeless on the ground.";
      }

      else {
         description = dstrview(message);
      }
   }

   g_outputString("\nYou see %s.  %s\n", dstrview(creature->title),
      description);
}

