
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


void attack(void *aggressor, enum EntityType aggressorType, void *defender,
enum EntityType defenderType, Object *weapon);

/******************************************************************************/

static int attackSuccess(void *aggressor, enum EntityType aggressorType,
void *defender, enum EntityType defenderType);

static int calcDamage(void *aggressor, enum EntityType aggressorType,
void *defender, enum EntityType defenderType, Object *weapon);

/******************************************************************************/

void attack(void *aggressor, enum EntityType aggressorType, void *defender,
enum EntityType defenderType, Object *weapon) {

   if (attackSuccess(aggressor, aggressorType, defender, defenderType)) {

      int damage = calcDamage(aggressor, aggressorType, defender, defenderType,
         weapon);

      // TODO: subtract damage from player's or creature's health

      g_outputString("You dealt a blow to %s!\n",
         dstrview(defenderType == entity_player ? ((Player *)defender)->name :
         ((Creature *)defender)->name));
   }

   else {
      g_outputString("Attack failed!\n");
   }

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

