
#include <stdio.h>
#include <stdlib.h>

#include "include/trogdor.h"
#include "include/parsexml.h"

#define GAME_FILE "game.xml"


/* initializes game data/assets such as rooms */
void initData();

/* initializes the rooms */
static void initRooms();

/* frees memory used by game data/assets */
void destroyData();


/* rooms that have been parsed from the XML game file */
RoomParsed **parsedRooms;

/* game objects that have been parsed from the XML game file */
ObjectParsed **parsedObjects;

/* all rooms in the game, with the pointer itself referencing "start" */
Room *rooms = NULL;


/* TODO: we should eventually add a parser to do custom room definitions */
void initData() {

   if (!parseGameFile(GAME_FILE)) {
      fprintf(stderr, "failed to parse game file %s\n", GAME_FILE);
      exit(EXIT_FAILURE);
   }

   // TODO

   initRooms();
}

static void initRooms() {

   Room *start;

   start = (Room *)malloc(sizeof(Room));

   start->description = "This is such a cool room!";
   start->north = NULL;
   start->south = NULL;
   start->east  = NULL;
   start->west  = NULL;

   rooms = start;

   Room *next;
   next = (Room *)malloc(sizeof(Room));

   next->description = "Ok, now you're in a dead end.  So you lose :)";
   next->north = NULL;
   next->south = start;
   next->east = NULL;
   next->west = NULL;

   // connect room "next" to room "start"
   start->north = next;

   return;
}


void destroyData() {

   // TODO: this doesn't work; only frees "start" room...
   free(rooms);
}

