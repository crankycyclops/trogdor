#ifndef COMBAT_H
#define COMBAT_H


#include "trogdor.h"
#include "room.h"
#include "object.h"
#include "creature.h"

/* by default, creatures are attackable */
#define DEFAULT_CREATURE_ATTACKABLE  1

extern void attack(void *aggressor, enum EntityType aggressorType,
void *defender, enum EntityType defenderType, Object *weapon,
int counterAttack);


#endif

