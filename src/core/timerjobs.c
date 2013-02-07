
#define TIMERJOBS_C

#include "include/timerjobs.h"


/* A timer job that will call attack() with the arguments specified in arg. 
   Id is the job's id. */
void timedAttack(unsigned long id, TimedAttackArgument *arg);

/******************************************************************************/

void timedAttack(unsigned long id, TimedAttackArgument *arg) {

   g_outputString("Timed attack stub!");
   return;
}

/******************************************************************************/
