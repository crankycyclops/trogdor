#ifndef COMBAT_H
#define COMBAT_H


#include "trogdor.h"
#include "room.h"
#include "object.h"
#include "creature.h"
#include "player.h"

/* by default, creatures are attackable */
#define DEFAULT_CREATURE_ATTACKABLE  1

/* Passed to any timer job that must carry out an attack */
typedef struct {
   void *attacker;
   void *defender;
   Object *weapon;
   enum EntityType attackerType;
   enum EntityType defenderType;
} TimedAttackArgument;

#ifndef COMBAT_C

/* Make aggressor attack defender with the specified weapon.  If counterAttack
   is true, the defender will respond with an attack of its own. */
extern void attack(void *aggressor, enum EntityType aggressorType,
void *defender, enum EntityType defenderType, Object *weapon,
int counterAttack);

/* A timer job that will call attack() with the arguments specified in arg. 
   Id is the job's id. */
extern void timedAttack(unsigned long id, TimedAttackArgument *arg);

#endif


#endif

