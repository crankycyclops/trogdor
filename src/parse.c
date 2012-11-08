#include <stdio.h>
#include <stdlib.h>
#include <dstring.h>
#include <glib.h>

#include "include/trogdor.h"
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
static void initObjects(Room *room, GArray objectNames);

/* called by initObjects(); builds the structure for an object */
static Object *initObject(ObjectParsed *objectParsed);

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
      fprintf(stderr, "failed to parse game file %s\n", filename);
      return 0;
   }

   /* make sure a room named "start" exists */
   if (NULL == g_hash_table_lookup(roomParsedTable, "start")) {
      fprintf(stderr, "room 'start' must be defined\n");
      return 0;
   }

   initRooms();

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

   // TODO: destroy strings inside hash
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
      g_hash_table_insert(rooms, dstrview(room->name), room);

      curParsedRoom = curParsedRoom->next;
   }

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
   room->description = roomParsed->description;

   /* rooms will be connected later by another function */
   room->north = NULL;
   room->south = NULL;
   room->east  = NULL;
   room->west  = NULL;

   initObjects(room, roomParsed->objects);

   return room;
}

/******************************************************************************/

/*
   start = (Room *)malloc(sizeof(Room));

   start->description = "This is such a cool room!";
   start->north = NULL;
   start->south = NULL;
   start->east  = NULL;
   start->west  = NULL;
   start->objectList = NULL;
   start->objectByName = NULL;

   // add to array of rooms and set current location to start
   location = start;
   g_array_append_val(rooms, start);

   Room *next;
   next = (Room *)malloc(sizeof(Room));

   next->description = "Ok, now you're in a dead end.  So you lose :)";
   next->north = NULL;
   next->south = start;
   next->east = NULL;
   next->west = NULL;
   next->objectList = NULL;
   next->objectByName = NULL;

   // connect room "next" to room "start"
   start->north = next;

   // add to array
   g_array_append_val(rooms, next);
*/


static void initObjects(Room *room, GArray objectNames) {

   // TODO
   return;
}

/******************************************************************************/

static Object *initObject(ObjectParsed *objectParsed) {

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

