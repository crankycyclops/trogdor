
#include <stdio.h>
#include <stdlib.h>

#include <lua.h>

#include "include/trogdor.h"
#include "include/object.h"
#include "include/room.h"
#include "include/creature.h"
#include "include/parse.h"
#include "include/parsexml.h"
#include "include/state.h"

#define GAME_FILE "game.xml"


/* initializes game data/assets such as rooms */
void initData();

/* frees memory used by game data/assets */
void destroyData();

/* iterates over all rooms, calling destroyRoom() on each */
static void destroyRooms();

/* frees memory associated with a room */
static void destroyRoom(Room *room);

/* iterates over all objects, calling destroyObject() on each */
static void destroyObjects();

/* frees memory associated with an object */
static void destroyObject(Object *object);


/* Points to whatever function we should use to read input from the user. */
dstring_t (*g_readCommand)();

/* Points to whatever function we should use to replace printf
   (this should be provided by the application making use of the core) */
int (*g_outputString)(const char *format, ...);

/* Points to whatever function we should use to replace fprintf(stderr,...)
   (this should be provided by the application making use of the core) */
int (*g_outputError)(const char *format, ...);


/* all rooms in the game, indexed by name */
GHashTable *g_rooms = NULL;

/* all objects in the game, indexed by name ONLY (no synonyms) */
GHashTable *g_objects = NULL;

/* all creatures in the game */
GHashTable *g_creatures = NULL;

/******************************************************************************/

void initData() {

   g_rooms = g_hash_table_new(g_str_hash, g_str_equal);
   g_objects = g_hash_table_new(g_str_hash, g_str_equal);
   g_creatures = g_hash_table_new(g_str_hash, g_str_equal);

   if (!parseGame(GAME_FILE)) {
      exit(EXIT_FAILURE);
   }

   /* initialize the game's state (hence, defined in state.c) */
   initGame();
}

/******************************************************************************/

void destroyData() {

   destroyRooms();
   destroyObjects();
}

/******************************************************************************/

static void destroyRooms() {

   GList *gRoomList = g_hash_table_get_values(g_rooms);
   GList *curRoom = gRoomList;

   /* free all rooms */
   while (NULL != curRoom) {
      destroyRoom((Room *)curRoom->data);
      curRoom = curRoom->next;
   }

   g_list_free(gRoomList);
}

/******************************************************************************/

static void destroyObjects() {

   GList *gObjectList = g_hash_table_get_values(g_objects);
   GList *curObject = gObjectList;

   /* free all game objects */
   while (NULL != curObject) {
      destroyObject((Object *)curObject->data);
      curObject = curObject->next;
   }

   g_list_free(gObjectList);
}

/******************************************************************************/

static void destroyRoom(Room *room) {

   int i;

   if (NULL != room->objectList) {
      g_list_free(room->objectList);
   }

   if (NULL != room->objectByName) {
      // TODO: we have to destroy each GList inside objectByName first!
      g_hash_table_destroy(room->objectByName);
   }

   dstrfree(&room->name);
   dstrfree(&room->title);
   dstrfree(&room->description);

   free(room);
}

/******************************************************************************/

static void destroyObject(Object *object) {

   // TODO: don't free object here, just dstring_t's inside it!

   if (object->lua != NULL) {
      lua_close(object->lua);
   }

   return;
}

