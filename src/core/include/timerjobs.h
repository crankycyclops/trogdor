#ifndef TIMERJOBS_H
#define TIMERJOBS_H


#include "trogdor.h"
#include "room.h"
#include "object.h"
#include "creature.h"
#include "player.h"
#include "timer.h"


/* Passed to any timer job that must carry out an attack */
typedef struct {
   void *attacker;
   void *defender;
   Object *weapon;
   enum EntityType attackerType;
   enum EntityType defenderType;
} TimedAttackArgument;


#ifndef TIMERJOBS_C

/* A timer job that will call attack() with the arguments specified in arg. 
   Id is the job's id. */
extern void timedAttack(unsigned long id, TimedAttackArgument *arg);

#endif


#endif

