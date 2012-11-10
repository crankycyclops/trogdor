
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

   /* initialize the game's state (hence, defined in state.c) */
   initGame();
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

      GList *curObject = room->objectList;

      while (curObject != NULL) {
         destroyObject((Object *)curObject->data);
         curObject = g_list_next(curObject);
      }

      g_list_free(room->objectList);
      room->objectList = NULL;

      // TODO: destroy room's object hash table
   }

   if (NULL != room->objectByName) {
      g_hash_table_destroy(room->objectByName);
   }

   free(room);
   return;
}

/******************************************************************************/

static void destroyObject(Object *object) {

   // TODO: don't free object here, just dstring_t's inside it!
   return;
}

