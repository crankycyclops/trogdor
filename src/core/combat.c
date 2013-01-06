
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
enum EntityType defenderType, Object *weapon);

/******************************************************************************/

static int attackSuccess(void *aggressor, enum EntityType aggressorType,
void *defender, enum EntityType defenderType);

static int calcDamage(void *aggressor, enum EntityType aggressorType,
void *defender, enum EntityType defenderType, Object *weapon);

/******************************************************************************/

// TODO: don't allow if creature already dead
void attack(void *aggressor, enum EntityType aggressorType, void *defender,
enum EntityType defenderType, Object *weapon) {

   // TODO: beforeAttack event

   if (defenderType == entity_player && !((Player *)defender)->state.alive ||
   defenderType == entity_creature && !((Creature *)defender)->state.alive) {
      g_outputString("%s is already dead.\n",
         dstrview(defenderType == entity_player ? ((Player *)defender)->name
         : ((Creature *)defender)->name));
   }

   else {

      if (attackSuccess(aggressor, aggressorType, defender, defenderType)) {

         // TODO: beforeAttackSuccess event

         int damage = calcDamage(aggressor, aggressorType, defender,
            defenderType, weapon);

         g_outputString("You dealt a blow to %s!\n",
            dstrview(defenderType == entity_player ? ((Player *)defender)->name
            : ((Creature *)defender)->name));

         removeHealth(defender, defenderType, damage, TRUE);

         // TODO: afterAttackSuccess event
      }

      else {
         // TODO: beforeAttackFailure event
         g_outputString("Attack failed!\n");
         // TODO: afterAttackFailure event
      }
   }

   // TODO: afterAttack event

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

