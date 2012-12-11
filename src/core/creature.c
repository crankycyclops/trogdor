
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

/* allocates memory for a creature */
Creature *creatureAlloc();

/* displays a creature (long or short) */
void displayCreature(Player *player, Creature *creature,
int showLongDescription);

/* describes a creature in detail */
static void describeCreature(Creature *creature);

/******************************************************************************/

Creature *creatureAlloc() {

   Creature *newcreature;

   newcreature = malloc(sizeof(Creature));
   if (NULL == newcreature) {
      PRINT_OUT_OF_MEMORY_ERROR;
   }

   /* initialize the creature's state */
   newcreature->state.seenByPlayers = g_hash_table_new(g_str_hash, g_str_equal);
   newcreature->state.alive = 1;
   newcreature->state.seenByAPlayer = 0;

   return newcreature;
}

/******************************************************************************/

void displayCreature(Player *player, Creature *creature,
int showLongDescription) {

   if (ALLOW_ACTION != event(player, "beforeDisplayCreature", creature)) {
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
      /* TODO: distinguish between alive and dead */
      g_outputString("\nYou see %s.\n",
         dstrview(creature->title));
   }

   event(player, "afterDisplayCreature", creature);
}

/******************************************************************************/

static void describeCreature(Creature *creature) {

   /* TODO: distinguish between alive and dead for descriptions */
   g_outputString("\nYou see %s.  %s\n", dstrview(creature->title),
      dstrview(creature->description));
}

