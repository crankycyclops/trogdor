
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

   if (entity_player != type && entity_creature != type) {
      g_outputError("Unsupported entity type in addHealth(). This is a bug.\n");
      return;
   }

   if (!event("beforeAddHealth", entity_player == type ? (Player *)entity : NULL,
   entity_creature == type ? (Creature *)entity : NULL, entity_creature, 0)) {
      return;
   }

   if (entity_player == type) {

      Player *player = (Player *)entity;

      player->state.health += up;
      if (!allowOverflow && player->state.health > player->maxHealth) {
         player->state.health = player->maxHealth;
      }
   }

   else {

      Creature *creature = (Creature *)entity;

      creature->state.health += up;
      if (!allowOverflow && creature->state.health > creature->maxHealth) {
         creature->state.health = creature->maxHealth;
      }
   }

   event("afterAddHealth", entity_player == type ? (Player *)entity : NULL,
      entity_creature == type ? (Creature *)entity : NULL, entity_creature, 0);

   return;
}

/******************************************************************************/

void removeHealth(void *entity, enum EntityType type, int down, int allowDeath) {

   int dies = 0;

   int health = entity_player == type ? ((Player *)entity)->state.health :
      ((Creature *)entity)->state.health;
   int maxHealth = entity_player == type ? ((Player *)entity)->maxHealth :
      ((Creature *)entity)->maxHealth;
   char *name = entity_player == type ? ((Player *)entity)->name :
      ((Creature *)entity)->name;

   if (entity_player != type && entity_creature != type) {
      g_outputError("Unsupported entity type in removeHealth(). This is a bug.\n");
      return;
   }

   if (!event("beforeRemoveHealth", entity_player == type ? (Player *)entity : NULL,
   entity_creature == type ? (Creature *)entity : NULL, entity_creature, 0)) {
      return;
   }

   g_outputString("%s loses %d health points.\n", dstrview(name), down);

   if (entity_player == type) {

      ((Player *)entity)->state.health -= down;
      health = ((Player *)entity)->state.health;

      if (((Player *)entity)->state.health <= 0) {
         dies = 1;
      }
   }

   else {

      ((Creature *)entity)->state.health -= down;
      health = ((Creature *)entity)->state.health;

      if (((Creature *)entity)->state.health <= 0) {
         dies = 1;
      }
   }

   if (dies && allowDeath) {
      die(entity, type);
   }

   else {
      g_outputString("%s has %d of %d possible health points.\n", dstrview(name),
         health, maxHealth);
   }

   event("afterRemoveHealth", entity_player == type ? (Player *)entity : NULL,
      entity_creature == type ? (Creature *)entity : NULL, entity_creature, 0);

   return;
}

/******************************************************************************/

void die(void *entity, enum EntityType type) {

   dstring_t message;

   if (entity_player != type && entity_creature != type) {
      g_outputError("Unsupported entity type in die(). This is a bug.\n");
      return;
   }

   if (!event("beforeDie", entity_player == type ? (Player *)entity : NULL,
   entity_creature == type ? (Creature *)entity : NULL, entity_creature, 0)) {
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
      g_outputString("%s dies.\n", dstrview(type == entity_player ?
         ((Player *)entity)->name : ((Creature *)entity)->name));
   }

   event("afterDie", entity_player == type ? (Player *)entity : NULL,
      entity_creature == type ? (Creature *)entity : NULL, entity_creature, 0);

   return;
}

