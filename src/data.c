
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

/* frees memory associated with a room */
static void destroyRoom(Room *room);

/* frees memory associated with an object */
static void destroyObject(Object *object);


/* all rooms in the game, indexed by room name */
GHashTable *rooms = NULL;

/******************************************************************************/

void initData() {

   rooms = g_hash_table_new(g_str_hash, g_str_equal);

   if (!parseGame(GAME_FILE)) {
      exit(EXIT_FAILURE);
   }
}

/******************************************************************************/

void destroyData() {

   // TODO: iterate over all rooms in hash table

   // TODO: free hash table itself
   return;
}

/******************************************************************************/

static void destroyRoom(Room *room) {

   int i;

   if (NULL != room->objectList) {
      for (i = 0; i < room->objectList->len; i++) {
         destroyObject(g_array_index(room->objectList, Object *, i));
      }
   }

   if (NULL != room->objectByName) {
      g_hash_table_destroy(room->objectByName);
   }

   free(room);
   return;
}

/******************************************************************************/

static void destroyObject(Object *object) {

   // TODO
   return;
}

