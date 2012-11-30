#include <stdio.h>
#include <stdlib.h>
#include <dstring.h>
#include <glib.h>

#include <lua51/lua.h>
#include <lua51/lualib.h>
#include <lua51/lauxlib.h>

#include "include/trogdor.h"
#include "include/object.h"
#include "include/room.h"
#include "include/data.h"
#include "include/state.h"
#include "include/parse.h"


/* entry point for parsing the game file */
int parseGame();

/* initializes the rooms */
static void initRooms();

/* called by initRooms(); builds the structure for a room */
static Room *initRoom(RoomParsed *roomParsed);

/* initialize game objects in a room from parsed data */
static void initObjects(Room *room, GArray *objectNames);

/* called by initObjects(); builds the structure for an object */
static Object *initObject(ObjectParsed *objectParsed);

/* connect rooms so that users can navigate north, south, etc. */
static void connectRooms();

/* prints parsed data for a room */
static void printParsedRoom(RoomParsed *room);

/* prints parsed data for an object */
static void printParsedObject(ObjectParsed *object);

/* initializes the parser */
static void initParser();

/* frees memory associated with the parser */
static void destroyParser();

/* initializes a Lua state with our C-to-Lua API */
static void initLua(lua_State *L);

/* loads a Lua script from a file */
static void loadScript(lua_State *L, char *filename);

/* primes Lua state so that all variables and functions get loaded */
static void primeLua(lua_State *L);


/* a lookup table for game objects being parsed */
GHashTable *objectParsedTable = NULL;

/* a lookup table for rooms being parsed */
GHashTable *roomParsedTable = NULL;

/* This gets set to 1 if we had trouble parsing a script.  Using this will allow
   us to delay exiting due to the error until we've parsed and reported errors
   for all possibly bad scripts. */
static int g_scriptError = 0;

/******************************************************************************/

int parseGame(const char *filename) {

   initParser();

   if (!parseGameFile(filename)) {
      fprintf(stderr, "failed to parse game file %s\n", filename);
      return 0;
   }

   /* make sure a room named "start" exists */
   if (NULL == g_hash_table_lookup(roomParsedTable, "start")) {
      fprintf(stderr, "room 'start' must be defined\n");
      return 0;
   }

   initRooms();

   /* make sure we didn't have any issues parsing Lua scripts */
   if (g_scriptError) {
      return 0;
   }

   destroyParser();
   return 1;
}

/******************************************************************************/

static void initParser() {

   objectParsedTable = g_hash_table_new(g_str_hash, g_str_equal);
   roomParsedTable   = g_hash_table_new(g_str_hash, g_str_equal);
}

/******************************************************************************/

static void destroyParser() {

   g_hash_table_destroy(objectParsedTable);
   g_hash_table_destroy(roomParsedTable);
}

/******************************************************************************/

static void initRooms() {

   Room *room;  /* actual room object */
   GList *parsedRoomList = g_hash_table_get_values(roomParsedTable);
   GList *curParsedRoom = parsedRoomList;

   /* iterate through each value in the rooms parsed table */
   while (NULL != curParsedRoom) {

      /* build the room structure and index it by name */
      room = initRoom((RoomParsed *)curParsedRoom->data);
      g_hash_table_insert(rooms, (char *)dstrview(room->name), room);

      curParsedRoom = curParsedRoom->next;
   }

   connectRooms();

   g_list_free(parsedRoomList);
   return;
}

/******************************************************************************/

static Room *initRoom(RoomParsed *roomParsed) {

   Room *room;  /* actual room object */

   /* initialize new room structure */
   room = malloc(sizeof(Room));
   if (NULL == room) {
      PRINT_OUT_OF_MEMORY_ERROR;
   }

   /* don't free these dstring_t objects when we free the parsed rooms table! */
   room->name = roomParsed->name;
   room->title = roomParsed->title;
   room->description = roomParsed->description;

   /* rooms will be connected later by another function */
   room->north = NULL;
   room->south = NULL;
   room->east  = NULL;
   room->west  = NULL;

   room->state.visitedByPlayer = 0;

   room->objectList = NULL;
   room->objectByName = NULL;
   initObjects(room, roomParsed->objects);

   return room;
}

/******************************************************************************/

static void initObjects(Room *room, GArray *objectNames) {

   int    i;
   Object *object;  /* actual object structure */

   // TODO: check to make sure these calls succeed
   room->objectByName = g_hash_table_new(g_str_hash, g_str_equal);
   room->objectList = NULL;

   /* instantiate each object in the room */
   for (i = 0; i < objectNames->len; i++) {

      int   j;
      GList *synonymList;

      dstring_t name = g_array_index(objectNames, dstring_t, i);
      ObjectParsed *curParsedObject = g_hash_table_lookup(objectParsedTable,
         dstrview(name));

      /* build the object */
      object = initObject(curParsedObject);

      /* add object to our global list */
      g_hash_table_insert(objects, (char *)dstrview(object->name), object);

      /* add object to whatever room it belongs in */
      room->objectList = g_list_append(room->objectList, object);

      /* add object to list of objects with same name / synonym */
      synonymList = g_hash_table_lookup(room->objectByName,
         (char *)dstrview(object->name));
      synonymList = g_list_append(synonymList, object);
      g_hash_table_insert(room->objectByName, (char *)dstrview(object->name),
         synonymList);

      /* we also want to index the object by its synonyms */
      for (j = 0; j < curParsedObject->synonyms->len; j++) {
         synonymList = g_hash_table_lookup(room->objectByName,
            (char *)dstrview(g_array_index(curParsedObject->synonyms, dstring_t,
            j)));
         synonymList = g_list_append(synonymList, object);
         g_hash_table_insert(room->objectByName,
            (char *)dstrview(g_array_index(curParsedObject->synonyms, dstring_t,
            j)), synonymList);
      }
   }

   return;
}

/******************************************************************************/

static Object *initObject(ObjectParsed *objectParsed) {

   int i;
   lua_State *L = NULL;
   Object *object;

   /* initialize new object structure */
   object = malloc(sizeof(Object));
   if (NULL == object) {
      PRINT_OUT_OF_MEMORY_ERROR;
   }

   object->name = objectParsed->name;
   object->description = objectParsed->description;
   object->weight = atoi(dstrview(objectParsed->weight));

   object->synonyms = objectParsed->synonyms;

   object->state.seenByPlayer = 0;
   object->state.takenByPlayer = 0;
   object->state.droppedByPlayer = 0;

   /* initialize any scripts attached to the object */
   for (i = 0; i < objectParsed->scripts->len; i++) {

      /* only initialize lua state if we have something to execute */
      if (NULL == L) {
         L = luaL_newstate();
         initLua(L);
      }

      #define SCRIPT_FILE (char *)dstrview(g_array_index(objectParsed->scripts, dstring_t, i))
      loadScript(L, SCRIPT_FILE);
      primeLua(L);
      #undef SCRIPT_FILE
   }

   object->lua = L;
   return object;
}

/******************************************************************************/

static void initLua(lua_State *L) {

   /* load standard library */
   luaL_openlibs(L);

   /* possible return values (see event.h and event.c for more details) */
   lua_pushboolean(L, 1);
   lua_setglobal(L, "SUPPRESS_ACTION");
   lua_pushboolean(L, 0);
   lua_setglobal(L, "ALLOW_ACTION");

   return;
}

/******************************************************************************/

static void loadScript(lua_State *L, char *filename) {

   int status;

   if (status = luaL_loadfile(L, filename)) {

      switch (status) {

         case LUA_ERRFILE:
            fprintf(stderr, "error: could not open %s\n", filename);
            g_scriptError = 1;
            break;

         case LUA_ERRSYNTAX:
            fprintf(stderr, "%s\n", lua_tostring(L, -1));
            g_scriptError = 1;
            break;

         case LUA_ERRMEM:
            PRINT_OUT_OF_MEMORY_ERROR;

         default:
            break;
      }
   }
}

/******************************************************************************/

/* TODO: this could probably be a macro instead... */
static void primeLua(lua_State *L) {

   if (lua_pcall(L, 0, 0, 0)) {
      fprintf(stderr, "%s\n", lua_tostring(L, -1));
      exit(EXIT_FAILURE);
   }
}

/******************************************************************************/

static void connectRooms() {

   GList *roomNames = g_hash_table_get_keys(rooms);
   GList *currentRoomName = roomNames;

   while (currentRoomName != NULL) {

      Room *room = g_hash_table_lookup(rooms, currentRoomName->data);
      Room *connected = NULL;
      RoomParsed *roomDefinition = g_hash_table_lookup(roomParsedTable,
         currentRoomName->data);

      if (NULL != roomDefinition->north) {

         connected = g_hash_table_lookup(rooms, dstrview(roomDefinition->north));

         /* game file specified a room that wasn't defined! */ 
         if (NULL == connected) {
            fprintf(stderr, "error: room '%s' connects to room '%s', but "
               "room '%s' doesn't exist!\n", dstrview(room->name),
               dstrview(roomDefinition->north), dstrview(roomDefinition->north));
            exit(EXIT_FAILURE);
         }

         room->north = connected;
      }

      if (NULL != roomDefinition->south) {

         connected = g_hash_table_lookup(rooms, dstrview(roomDefinition->south));

         if (NULL == connected) {
            fprintf(stderr, "error: room '%s' connects to room '%s', but "
               "room '%s' doesn't exist!\n", dstrview(room->name),
               dstrview(roomDefinition->south), dstrview(roomDefinition->south));
            exit(EXIT_FAILURE);
         }

         room->south = connected;
      }

      if (NULL != roomDefinition->east) {

         connected = g_hash_table_lookup(rooms, dstrview(roomDefinition->east));

         if (NULL == connected) {
            fprintf(stderr, "error: room '%s' connects to room '%s', but "
               "room '%s' doesn't exist!\n", dstrview(room->name),
               dstrview(roomDefinition->east), dstrview(roomDefinition->east));
            exit(EXIT_FAILURE);
         }

         room->east = connected;
      }

      if (NULL != roomDefinition->west) {

         connected = g_hash_table_lookup(rooms, dstrview(roomDefinition->west));

         if (NULL == connected) {
            fprintf(stderr, "error: room '%s' connects to room '%s', but "
               "room '%s' doesn't exist!\n", dstrview(room->name),
               dstrview(roomDefinition->west), dstrview(roomDefinition->west));
            exit(EXIT_FAILURE);
         }

         room->west = connected;
      }

      currentRoomName = currentRoomName->next;
   }

   g_list_free(roomNames);
   return;
}

/******************************************************************************/

static void printParsedObject(ObjectParsed *object) {

   printf("Name: %s\n", dstrview(object->name));
   printf("Description: %s\n", dstrview(object->description));
}

/******************************************************************************/

static void printParsedRoom(RoomParsed *room) {

   int i = 0;

   printf("Name: %s\n", dstrview(room->name));
   printf("Description: %s\n", dstrview(room->description));

   if (NULL != room->north) {
      printf("North: %s\n", dstrview(room->north));
   }

   if (NULL != room->south) {
      printf("South: %s\n", dstrview(room->south));
   }

   if (NULL != room->east) {
      printf("East: %s\n", dstrview(room->east));
   }

   if (NULL != room->west) {
      printf("West: %s\n", dstrview(room->west));
   }

   if (NULL != room->objects) {
      printf("Objects: ");
      for (i = 0; i < room->objects->len; i++) {
         printf("%s, ", dstrview(g_array_index(room->objects, dstring_t, i)));
      }

      printf("\n");
   }

   printf("\n");
}

