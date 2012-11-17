
#ifndef STATE_H
#define STATE_H


#include <glib.h>
#include "trogdor.h"
#include "room.h"

/* our current location in the game */
extern Room *location;

/* a doubly linked list of all items in the user's inventory */
extern GList *inventory;

/* hash of object names and synonyms for quick lookup by name */
extern GHashTable *inventoryByName;

/* initialize the game's state */
extern void initGame();


#endif

