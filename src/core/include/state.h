
#ifndef STATE_H
#define STATE_H


#include <glib.h>
#include <dstring.h>

#include "trogdor.h"

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

/* returns 1 if we're running a game and 0 if not */
extern int isInGame();

/* initialize the game's state */
extern void initGame();

/* destroys resources and quits the game */
extern void destroyGame();

/* keeps data consistent between threads */
extern pthread_mutex_t resourceMutex;

#endif


#endif

