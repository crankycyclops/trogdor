
#define TIMERJOBS_C

#include "include/timerjobs.h"


/* A timer job that will call attack() with the arguments specified in arg. 
   Id is the job's id. */
void timedAttack(unsigned long id, TimedAttackArgument *arg);

/******************************************************************************/

void timedAttack(unsigned long id, TimedAttackArgument *arg) {

   TimedAttackArgument *params = (TimedAttackArgument *)arg;

   int attackerAlive;
   int defenderAlive;

   Room *attackerLocation;
   Room *defenderLocation;

   switch (params->attackerType) {

      case entity_player:
         attackerAlive = ((Player *)params->attacker)->state.alive;
         attackerLocation = ((Player *)params->attacker)->location;
         break;

      case entity_creature:
         attackerAlive = ((Creature *)params->attacker)->state.alive;
         attackerLocation = ((Creature *)params->attacker)->location;
         break;

      default:
         g_outputError("Passed unsupported attacker entity type to "
            "timedAttack.  This is a bug.\n");
         return;
   }

   switch (params->defenderType) {

      case entity_player:
         defenderAlive = ((Player *)params->defender)->state.alive;
         defenderLocation = ((Player *)params->defender)->location;
         break;

      case entity_creature:
         defenderAlive = ((Creature *)params->defender)->state.alive;
         defenderLocation = ((Creature *)params->defender)->location;
         break;

      default:
         g_outputError("Passed unsupported defender entity type to "
            "timedAttack.  This is a bug.\n");
         return;
   }

   // cancel the auto-attack if either entity is dead
   if (!attackerAlive || !defenderAlive) {
      deregisterTimedJob(id);
   }

   // attacker and defender must be in the same room
   else if (attackerLocation != defenderLocation) {
      deregisterTimedJob(id);
   }

   else {
      g_outputString("Timed attack stub!");
   }

   return;
}

/******************************************************************************/
