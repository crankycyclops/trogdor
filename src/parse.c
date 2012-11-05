#include <stdio.h>
#include <dstring.h>

#include "include/parse.h"


/* entry point for parsing the game file */
int parseGame();

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

   destroyParser();
   return 1;
}


static void initParser() {

   objectParsedTable = g_hash_table_new(g_str_hash, g_str_equal);
   roomParsedTable   = g_hash_table_new(g_str_hash, g_str_equal);
}


static void destroyParser() {

   // TODO: destroy strings inside hash
   g_hash_table_destroy(objectParsedTable);
   g_hash_table_destroy(roomParsedTable);
}


static void printParsedObject(ObjectParsed *object) {

   printf("Name: %s\n", dstrview(object->name));
   printf("Description: %s\n", dstrview(object->description));
}


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
      for (i = 0; room->objects[i] != NULL; i++) {
         printf("%s, ", dstrview(room->objects[i]));
      }

      printf("\n");
   }

   printf("\n");
}

