#include <stdio.h>
#include <stdlib.h>

#include "include/trogdor.h"
#include "include/object.h"
#include "include/room.h"
#include "include/data.h"
#include "include/state.h"
#include "include/construct.h"
#include "include/script.h"
#include "include/event.h"


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

/* a lookup table for game objects being parsed */
GHashTable *objectParsedTable = NULL;

/* a lookup table for rooms being parsed */
GHashTable *roomParsedTable = NULL;

/******************************************************************************/

int parseGame(const char *filename) {

   initParser();

   if (!parseGameFile(filename)) {
      g_outputError("failed to parse game file %s\n", filename);
      return 0;
   }

   /* make sure a room named "start" exists */
   if (NULL == g_hash_table_lookup(roomParsedTable, "start")) {
      g_outputError("room 'start' must be defined\n");
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
      g_hash_table_insert(g_rooms, (char *)dstrview(room->name), room);

      curParsedRoom = curParsedRoom->next;
   }

   connectRooms();

   g_list_free(parsedRoomList);
   return;
}

/******************************************************************************/

static Room *initRoom(RoomParsed *roomParsed) {

   Room *room = roomAlloc();  /* actual room object */

   /* don't free these dstring_t objects when we free the parsed rooms table! */
   room->name = roomParsed->name;
   room->title = roomParsed->title;
   room->description = roomParsed->description;

   /* rooms will be connected later by another function */
   room->north = NULL;
   room->south = NULL;
   room->east  = NULL;
   room->west  = NULL;

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
      g_hash_table_insert(g_objects, (char *)dstrview(object->name), object);

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
   object->takeable = atoi(dstrview(objectParsed->takeable));
   object->droppable = atoi(dstrview(objectParsed->droppable));

   object->synonyms = objectParsed->synonyms;

   object->state.seenByPlayers = g_hash_table_new(g_str_hash, g_str_equal);
   object->state.takenByPlayers = g_hash_table_new(g_str_hash, g_str_equal);
   object->state.droppedByPlayers = g_hash_table_new(g_str_hash, g_str_equal);

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
      if (loadScript(L, SCRIPT_FILE)) {
         primeLua(L);
      }
      #undef SCRIPT_FILE
   }

   object->lua = L;
   return object;
}

/******************************************************************************/

static void connectRooms() {

   GList *roomNames = g_hash_table_get_keys(g_rooms);
   GList *currentRoomName = roomNames;

   while (currentRoomName != NULL) {

      Room *room = g_hash_table_lookup(g_rooms, currentRoomName->data);
      Room *connected = NULL;
      RoomParsed *roomDefinition = g_hash_table_lookup(roomParsedTable,
         currentRoomName->data);

      if (NULL != roomDefinition->north) {

         connected = g_hash_table_lookup(g_rooms, dstrview(roomDefinition->north));

         /* game file specified a room that wasn't defined! */ 
         if (NULL == connected) {
            g_outputError("error: room '%s' connects to room '%s', but "
               "room '%s' doesn't exist!\n", dstrview(room->name),
               dstrview(roomDefinition->north), dstrview(roomDefinition->north));
            exit(EXIT_FAILURE);
         }

         room->north = connected;
      }

      if (NULL != roomDefinition->south) {

         connected = g_hash_table_lookup(g_rooms, dstrview(roomDefinition->south));

         if (NULL == connected) {
            g_outputError("error: room '%s' connects to room '%s', but "
               "room '%s' doesn't exist!\n", dstrview(room->name),
               dstrview(roomDefinition->south), dstrview(roomDefinition->south));
            exit(EXIT_FAILURE);
         }

         room->south = connected;
      }

      if (NULL != roomDefinition->east) {

         connected = g_hash_table_lookup(g_rooms, dstrview(roomDefinition->east));

         if (NULL == connected) {
            g_outputError("error: room '%s' connects to room '%s', but "
               "room '%s' doesn't exist!\n", dstrview(room->name),
               dstrview(roomDefinition->east), dstrview(roomDefinition->east));
            exit(EXIT_FAILURE);
         }

         room->east = connected;
      }

      if (NULL != roomDefinition->west) {

         connected = g_hash_table_lookup(g_rooms, dstrview(roomDefinition->west));

         if (NULL == connected) {
            g_outputError("error: room '%s' connects to room '%s', but "
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

