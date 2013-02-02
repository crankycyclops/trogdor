
#define COMBAT_C

#include <stdio.h>
#include <stdlib.h>

#include <glib.h>

#include "include/trogdor.h"
#include "include/object.h"
#include "include/room.h"
#include "include/creature.h"
#include "include/event.h"
#include "include/messages.h"
#include "include/data.h"
#include "include/health.h"
#include "include/attributes.h"
#include "include/utility.h"


void attack(void *aggressor, enum EntityType aggressorType, void *defender,
enum EntityType defenderType, Object *weapon, int counterAttack);

/******************************************************************************/

static int attackSuccess(void *aggressor, enum EntityType aggressorType,
void *defender, enum EntityType defenderType);

static int calcDamage(void *aggressor, enum EntityType aggressorType,
void *defender, enum EntityType defenderType, Object *weapon);

/******************************************************************************/

void attack(void *aggressor, enum EntityType aggressorType, void *defender,
enum EntityType defenderType, Object *weapon, int counterAttack) {

   if (entity_player == aggressorType) {
      addEventListener(((Player *)aggressor)->events);
   } else {
      addEventListener(((Creature *)aggressor)->events);
   }

   if (entity_player == defenderType) {
      addEventListener(((Player *)defender)->events);
   } else {
      addEventListener(((Creature *)defender)->events);
   }

   if (NULL != weapon) {
      addEventListener(weapon->events);
   }

   if (!event("beforeAttack", 3, entity_player == aggressorType ?
   eventArgPlayer(aggressor) : eventArgCreature(aggressor),
   entity_player == defenderType ? eventArgPlayer(defender) :
   eventArgCreature(defender), eventArgObject(weapon))) {
      return;
   }

   if (defenderType == entity_player && !((Player *)defender)->state.alive ||
   defenderType == entity_creature && !((Creature *)defender)->state.alive) {
      g_outputString("%s is already dead.\n",
         dstrview(defenderType == entity_player ? ((Player *)defender)->name
         : ((Creature *)defender)->name));
   }

   else {

      if (entity_player == aggressorType) {
         addEventListener(((Player *)aggressor)->events);
      } else {
         addEventListener(((Creature *)aggressor)->events);
      }

      if (entity_player == defenderType) {
         addEventListener(((Player *)defender)->events);
      } else {
         addEventListener(((Creature *)defender)->events);
      }

      if (NULL != weapon) {
         addEventListener(weapon->events);
      }

      if (attackSuccess(aggressor, aggressorType, defender, defenderType)) {

         int damage = calcDamage(aggressor, aggressorType, defender,
            defenderType, weapon);

         g_outputString("%s dealt a blow to %s!\n",
            dstrview(aggressorType == entity_player ? ((Player *)aggressor)->name
            : ((Creature *)aggressor)->name),
            dstrview(defenderType == entity_player ? ((Player *)defender)->name
            : ((Creature *)defender)->name));

         removeHealth(defender, defenderType, damage, TRUE);

         event("attackSuccess", 3, entity_player == aggressorType ?
            eventArgPlayer(aggressor) : eventArgCreature(aggressor),
            entity_player == defenderType ? eventArgPlayer(defender) :
            eventArgCreature(defender), eventArgObject(weapon));
      }

      else {
         g_outputString("Attack failed!\n");
         event("attackFailure", 3, entity_player == aggressorType ?
            eventArgPlayer(aggressor) : eventArgCreature(aggressor),
            entity_player == defenderType ? eventArgPlayer(defender) :
            eventArgCreature(defender), eventArgObject(weapon));
      }
   }

   if (entity_player == aggressorType) {
      addEventListener(((Player *)aggressor)->events);
   } else {
      addEventListener(((Creature *)aggressor)->events);
   }

   if (entity_player == defenderType) {
      addEventListener(((Player *)defender)->events);
   } else {
      addEventListener(((Creature *)defender)->events);
   }

   if (NULL != weapon) {
      addEventListener(weapon->events);
   }

   /* We won't continue on with the possibility of a counter attack unless
      the event handler says it's ok to continue. */
   if (!event("afterAttack", 3, entity_player == aggressorType ?
   eventArgPlayer(aggressor) : eventArgCreature(aggressor),
   entity_player == defenderType ? eventArgPlayer(defender) :
   eventArgCreature(defender), eventArgObject(weapon))) {
      return;
   }

   /* If the entity being attacked is a creature, and it's configured to
      fight back, then for each attempted attack, the creature should counter
      with an attack of its own. */
   if (counterAttack) {
      if (entity_creature == defenderType && ((Creature *)defender)->counterattack) {
         // TODO: should this be timed?
         if (((Creature *)defender)->state.alive) {
            g_outputString("\n%s fights back.\n",
               dstrview(((Creature *)defender)->name));
            // TODO: creature weapon selection?  Random?  Hmm...
            attack(defender, defenderType, aggressor, aggressorType, NULL, FALSE);
         }
      }
   }

   return;
}

/******************************************************************************/

static int attackSuccess(void *aggressor, enum EntityType aggressorType,
void *defender, enum EntityType defenderType) {

   double p;              /* probability of success */
   double woundRate;      /* max probability of defender being hit by anyone */
   double dice;           /* random number */

   Attributes aggressorAttrs = entity_player == aggressorType ?
      ((Player *)aggressor)->attributes : ((Creature *)aggressor)->attributes;

   dice = (double)rand() / RAND_MAX;

   woundRate = defenderType == entity_player ? ((Player *)defender)->woundRate :
      ((Creature *)defender)->woundRate;

   p = CLIP(calcStrengthFactor(aggressorAttrs) * (woundRate / 2) + (woundRate / 2),
      0.0, woundRate);

   if (dice < p) {
      return TRUE;
   }

   else {
      return FALSE;
   }
}

/******************************************************************************/

static int calcDamage(void *aggressor, enum EntityType aggressorType,
void *defender, enum EntityType defenderType, Object *weapon) {

   // TODO: make this configurable
   /* damage done without a weapon */
   int bareHands = 5;
   Attributes attributes = entity_player == aggressorType ?
      ((Player *)aggressor)->attributes : ((Creature *)aggressor)->attributes;
   int damage = (int)(bareHands * calcStrengthFactor(attributes)) < 1 ?
         1 : (int)(bareHands * calcStrengthFactor(attributes));

   if (NULL != weapon) {
      damage += weapon->damage;
   }

   return damage;
}

