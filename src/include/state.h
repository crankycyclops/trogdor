
#ifndef STATE_H
#define STATE_H


#include <glib.h>
#include "trogdor.h"
#include "room.h"

typedef struct inventory {

   /* list and by-name index of object pointers */
   GHashTable  *byName;
   GList       *list;

   /* current weight of inventory */
   int weight;

   /* inventory can't weight more than this value (if 0, unlimited) */
   int maxWeight;

} Inventory;


#ifndef STATE_C

/* our current location in the game */
extern Room *location;

/* list of player-owned objects */
extern Inventory inventory;

/* initialize the game's state */
extern void initGame();

#endif


#endif

