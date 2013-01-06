#ifndef HEALTH_H
#define HEALTH_H


#include "trogdor.h"
#include "room.h"
#include "object.h"
#include "creature.h"
#include "player.h"

#define DEFAULT_PLAYER_HEALTH       0
#define DEFAULT_PLAYER_MAXHEALTH    0
#define DEFAULT_PLAYER_ALIVE        1

#define DEFAULT_CREATURE_HEALTH     0
#define DEFAULT_CREATURE_MAXHEALTH  0
#define DEFAULT_CREATURE_ALIVE      1

/* Adds health to a player or creature.  Up is the amount of health to add.
   If allowOverflow is true, up will always be added to the current health,
   even if it means going over maxHealth.  If this is false, the health
   will be capped at maxHealth. */
extern void addHealth(void *entity, enum EntityType type, int up,
int allowOverflow);

/* Removes health from a player or creature.  Down is the amount to remove.
   If allowDeath is true and the health goes down to or below 0, die() will
   be called to kill the player or creature. */
extern void removeHealth(void *entity, enum EntityType type, int down,
int allowDeath);

/* Kills the creature or player. */
extern void die(void *entity, enum EntityType type);


#endif
