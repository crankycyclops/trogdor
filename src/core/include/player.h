#ifndef PLAYER_H
#define PLAYER_H


#include <glib.h>

#include "object.h"
#include "state.h"

typedef struct player {

   dstring_t   name;

   Room       *location;
   Inventory   inventory;

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

