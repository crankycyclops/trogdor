
#ifndef DATA_H
#define DATA_H


#include <glib.h>
#include "parsexml.h"

/* objects in the game */
extern GHashTable *objects;

/* all rooms in the game, with the pointer itself referencing "start" */
extern Room *rooms;

/* initializes game data/assets such as rooms */
extern void initData();

/* frees memory used by game data/assets */
extern void destroyData();


#endif

