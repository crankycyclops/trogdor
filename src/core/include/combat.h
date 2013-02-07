#ifndef COMBAT_H
#define COMBAT_H


#include "trogdor.h"
#include "room.h"
#include "object.h"
#include "creature.h"
#include "player.h"

/* by default, creatures are attackable */
#define DEFAULT_CREATURE_ATTACKABLE  1


#ifndef COMBAT_C

/* Make aggressor attack defender with the specified weapon.  If counterAttack
   is true, the defender will respond with an attack of its own. */
extern void attack(void *aggressor, enum EntityType aggressorType,
void *defender, enum EntityType defenderType, Object *weapon,
int counterAttack);

#endif


#endif

