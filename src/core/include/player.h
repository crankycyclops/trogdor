#ifndef PLAYER_H
#define PLAYER_H


#include <glib.h>

#include "object.h"
#include "attributes.h"
#include "state.h"

/* by default, a player is hit during combat no more than 50% of the time */
#define DEFAULT_PLAYER_WOUNDRATE 0.5

typedef struct {
   int health;  /* how many health points the player has */
   int alive;   /* true if the player is alive and false if dead */
} PlayerState;

// TODO: add messaging to player structure?
typedef struct player {

   dstring_t   name;

   Room         *location;
   Inventory    inventory;
   PlayerState  state;
   Attributes   attributes;

   /* 0 means the player is immortal (unless die() is called manually...) */
   int maxHealth;

   /* maximum probability of being hit when attacked */
   double woundRate;

   /* event handlers */
   GHashTable     *events;

} Player;

#ifndef PLAYER_C

/* used to model a new player's default settings */
extern Player g_playerConfig;

/* player constructor */
extern Player *createPlayer(char *name);

/* player destructor */
extern void destroyPlayer(Player *player);

#endif

#endif

