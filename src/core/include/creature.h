#ifndef CREATURE_H
#define CREATURE_H


#include <dstring.h>
#include <lua.h>

#include "object.h"
#include "attributes.h"
#include "messages.h"
#include "eventlist.h"


#define CREATURE_ALLEGIANCE_FRIEND  0
#define CREATURE_ALLEGIANCE_ENEMY   1
#define CREATURE_ALLEGIANCE_NEUTRAL 2

/* by default, a creature will be successfully hit when attacked no more than
   50% of the time */
#define DEFAULT_CREATURE_WOUNDRATE  0.5

/* by default, enemy creatures will fight back when attacked */
// TODO: should friendly,neutral creatures attack back by default?
#define DEFAULT_CREATURE_COUNTERATTACK 1

/* represents the state of a creature */
typedef struct creatureState {

   GHashTable *seenByPlayers;    /* all players that have seen the creature */

   int health;          /* how many health points the creature has */
   int alive;           /* 1 if creature is alive and 0 if it's dead */
   int seenByAPlayer;   /* whether creature has been seen by any player */

} CreatureState;

typedef struct creature {

   dstring_t name;           /* unique identifier */
   dstring_t title;          /* what the creature is called */
   dstring_t description;    /* description of the creature */

   Messages messages;        /* hash table of custom messages */
   Attributes attributes;    /* strength, dexterity, intelligence, etc. */
   CreatureState state;      /* creature's state */
   Inventory inventory;      /* objects owned by the creature */

   int    allegiance;        /* whether the creature is a friend, enemy or neutral */
   int    attackable;        /* whether or not creature can be attacked */
   int    counterattack;     /* whether or not creature will respond to attacks */
   double woundRate;         /* maximum probability of being hit during combat */

   int maxHealth;            /* max health points (0 means immortal) */

   EventHandlerList *events; /* event handlers */
   lua_State *L;             /* creature's scripting environment */

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

