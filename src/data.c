
#include <stdio.h>
#include <stdlib.h>
#include <glib.h>

#include "include/trogdor.h"
#include "include/parse.h"
#include "include/parsexml.h"

#define GAME_FILE "game.xml"


/* initializes game data/assets such as rooms */
void initData();

/* frees memory used by game data/assets */
void destroyData();


/* game objects, indexed by name */
GHashTable *objects = NULL;

/* all rooms in the game, with the pointer itself referencing "start" */
Room *rooms = NULL;


void initData() {

   if (!parseGame(GAME_FILE)) {
      exit(EXIT_FAILURE);
   }
}


void destroyData() {

   // TODO: this doesn't work; only frees "start" room...
   free(rooms);
}

