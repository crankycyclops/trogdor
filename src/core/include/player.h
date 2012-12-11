#ifndef PLAYER_H
#define PLAYER_H


#include <glib.h>

#include "object.h"
#include "state.h"

typedef struct player {

   dstring_t   name;

   Room       *location;
   Inventory   inventory;

} Player;

#ifndef PLAYER_C

/* used to model a new player's default settings */
extern Player g_playerConfig;

/* creates a new player */
extern Player *createPlayer(char *name);

#endif

#endif

