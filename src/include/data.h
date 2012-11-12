
#ifndef DATA_H
#define DATA_H


#include <glib.h>
#include "parsexml.h"


/* all rooms in the game, indexed by name */
extern GHashTable *rooms;

/* all objects in the game, indexed by name ONLY (no synonyms) */
extern GHashTable *objects;

/* initializes game data/assets such as rooms */
extern void initData();

/* frees memory used by game data/assets */
extern void destroyData();


#endif

