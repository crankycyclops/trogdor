#ifndef COMBAT_H
#define COMBAT_H


#include "trogdor.h"
#include "room.h"
#include "object.h"
#include "creature.h"

extern void attack(void *aggressor, enum EntityType aggressorType,
void *defender, enum EntityType defenderType, Object *weapon);


#endif

