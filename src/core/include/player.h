#ifndef PLAYER_H
#define PLAYER_H


#include <glib.h>

#include "object.h"
#include "state.h"

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

   /* 0 means the player is immortal (unless die() is called manually...) */
   int maxHealth;

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

