
#define HEALTH_C

#include <stdio.h>
#include <stdlib.h>

#include "include/trogdor.h"
#include "include/object.h"
#include "include/room.h"
#include "include/creature.h"
#include "include/event.h"
#include "include/messages.h"
#include "include/data.h"


/******************************************************************************/

/* Adds health to a player or creature.  Up is the amount of health to add.
   If allowOverflow is true, up will always be added to the current health,
   even if it means going over maxHealth.  If this is false, the health
   will be capped at maxHealth. */
void addHealth(void *entity, enum EntityType type, int up, int allowOverflow);

/* Removes health from a player or creature.  Down is the amount to remove.
   If allowDeath is true and the health goes down to or below 0, die() will
   be called to kill the player or creature. */
void removeHealth(void *entity, enum EntityType type, int down, int allowDeath);

/* Kills the creature or player. */
void die(void *entity, enum EntityType type);

/******************************************************************************/

void addHealth(void *entity, enum EntityType type, int up, int allowOverflow) {

   // TODO
   return;
}

/******************************************************************************/

void removeHealth(void *entity, enum EntityType type, int down, int allowDeath) {

   // TODO
   return;
}

/******************************************************************************/

void die(void *entity, enum EntityType type) {

   dstring_t message;

   if (!event("beforeDie", entity_player == type ? (Player *)entity : NULL,
   entity_creature == type ? (Creature *)entity : NULL, entity_creature, 0)) {
      return;
   }

   if (entity_player != type && entity_creature != type) {
      g_outputError("Unsupported entity type in die().  This is a bug.\n");
      return;
   }

   // TODO: add messaging to player structure?
   message = entity_creature == type ?
      getMessage(((Creature *)entity)->messages, "die") : NULL;

   if (entity_player == type) {
      ((Player *)entity)->state.health = 0;
      ((Player *)entity)->state.alive = 0;
   } else {
      ((Creature *)entity)->state.health = 0;
      ((Creature *)entity)->state.alive = 0;
   }

   if (NULL != message) {
      g_outputString("%s\n", dstrview(message));
   }

   else {
      g_outputString("%s dies.\n", type == entity_player ?
         ((Player *)entity)->name : ((Creature *)entity)->name);
   }

   event("afterDie", entity_player == type ? (Player *)entity : NULL,
      entity_creature == type ? (Creature *)entity : NULL, entity_creature, 0);

   return;
}

