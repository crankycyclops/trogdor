
#include <stdio.h>
#include <stdlib.h>
#include <glib.h>

#include "include/trogdor.h"
#include "include/parse.h"
#include "include/parsexml.h"

#define GAME_FILE "game.xml"


/* initializes game data/assets such as rooms */
void initData();

/* initializes the rooms */
static void initRooms();

/* frees memory used by game data/assets */
void destroyData();

/* prints parsed data for all rooms */
static void printParsedRooms();

/* prints parsed data for all objects */
static void printParsedObjects();


/* all rooms in the game, with the pointer itself referencing "start" */
Room *rooms = NULL;


void initData() {

   initParser();

   if (!parseGameFile(GAME_FILE)) {
      fprintf(stderr, "failed to parse game file %s\n", GAME_FILE);
      exit(EXIT_FAILURE);
   }

   destroyParser();
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

