#ifndef CREATURE_H
#define CREATURE_H


#include <dstring.h>
#include <lua.h>

#include "object.h"
#include "messages.h"


/* represents the state of a creature */
typedef struct creatureState {

   GHashTable *seenByPlayers;    /* all players that have seen the creature */

   int alive;           /* 1 if creature is alive and 0 if it's dead */
   int seenByAPlayer;   /* whether creature has been seen by any player */

} CreatureState;

typedef struct creature {

   dstring_t name;         /* unique identifier */
   dstring_t title;        /* what the creature is called */
   dstring_t description;  /* description of the creature */
   dstring_t deadDesc;     /* description of the creature when dead (optional) */

   Messages messages;      /* hash table of custom messages */
   CreatureState state;    /* creature's state */
   Inventory inventory;    /* objects owned by the creature */

   /* Lua state containing scripted functions */
   lua_State *lua;

} Creature;


#ifndef CREATURE_C

/* Allocates memory for a creature.  If initMessages is true, we allocate memory
   for the messages structure.  Otherwise, just set it to NULL. */
extern Creature *createCreature(int initMessages);

/* frees all memory allocated for a creature */
extern void destroyCreature(Creature *creature);

/* displays a creature (long or short) */
extern void displayCreature(Player *player, Creature *creature,
int showLongDescription);

#endif


#endif

