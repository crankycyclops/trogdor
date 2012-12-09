#ifndef CREATURE_H
#define CREATURE_H


#include <dstring.h>
#include <lua.h>


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

   CreatureState state;    /* creature's state */

   GList *objects;         /* objects owned by the creature */

   /* Lua state containing scripted functions */
   lua_State *lua;

} Creature;


#ifndef CREATURE_C

/* allocates memory for a creature */
extern Creature *creatureAlloc();

#endif


#endif

