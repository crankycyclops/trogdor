
#define COMBAT_C

#include <stdio.h>
#include <stdlib.h>

#include "include/trogdor.h"
#include "include/object.h"
#include "include/room.h"
#include "include/creature.h"
#include "include/event.h"
#include "include/messages.h"
#include "include/data.h"
#include "include/health.h"


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

   if (attackSuccess(aggressor, aggressorType, defender, defenderType)) {

      // TODO: beforeAttackSuccess event

      int damage = calcDamage(aggressor, aggressorType, defender, defenderType,
         weapon);

      g_outputString("You dealt a blow to %s!\n",
         dstrview(defenderType == entity_player ? ((Player *)defender)->name :
         ((Creature *)defender)->name));

      removeHealth(defender, defenderType, damage, TRUE);

      // TODO: afterAttackSuccess event
   }

   else {
      // TODO: beforeAttackFailure event
      g_outputString("Attack failed!\n");
      // TODO: afterAttackFailure event
   }

   // TODO: afterAttack event

   return;
}

/******************************************************************************/

static int attackSuccess(void *aggressor, enum EntityType aggressorType,
void *defender, enum EntityType defenderType) {

   double p;              /* probability of success */   
   double dice;           /* random number */

   dice = (double)rand() / RAND_MAX;

   // TODO
   p = 0.5;

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

   // TODO
   return 1;
}

