
#ifndef DATA_H
#define DATA_H


#include "parsexml.h"

/* all rooms in the game, with the pointer itself referencing "start" */
extern Room *rooms;

/* rooms that have been parsed from the XML game file */
extern RoomParsed **parsedRooms;

/* game objects that have been parsed from the XML game file */
extern ObjectParsed **parsedObjects;

/* initializes game data/assets such as rooms */
extern void initData();

/* frees memory used by game data/assets */
extern void destroyData();


#endif

