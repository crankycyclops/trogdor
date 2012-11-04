#include <stdio.h>
#include <dstring.h>

#include "include/parse.h"


/* entry point for parsing the game file */
int parseGame();

/* prints parsed data for all rooms */
static void printParsedRooms();

/* prints parsed data for all objects */
static void printParsedObjects();

/* initializes the parser */
static void initParser();

/* frees memory associated with the parser */
static void destroyParser();


/* rooms that have been parsed from the XML game file */
RoomParsed **parsedRooms = NULL;

/* game objects that have been parsed from the XML game file */
ObjectParsed **parsedObjects = NULL;

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
   if (!g_hash_table_contains(roomParsedTable, "start")) {
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


static void printParsedObjects() {

   int i;

   for (i = 0; parsedObjects[i] != NULL; i++) {

      printf("Object %d:\n", i);
      printf("Name: %s\n", dstrview(parsedObjects[i]->name));
      printf("Description: %s\n", dstrview(parsedObjects[i]->description));
   }
}


static void printParsedRooms() {

   int i;

   for (i = 0; parsedRooms[i] != NULL; i++) {

      int j = 0;

      printf("Room %d:\n", i);
      printf("Name: %s\n", dstrview(parsedRooms[i]->name));
      printf("Description: %s\n", dstrview(parsedRooms[i]->description));

      if (NULL != parsedRooms[i]->north) {
         printf("North: %s\n", dstrview(parsedRooms[i]->north));
      }

      if (NULL != parsedRooms[i]->south) {
         printf("South: %s\n", dstrview(parsedRooms[i]->south));
      }

      if (NULL != parsedRooms[i]->east) {
         printf("East: %s\n", dstrview(parsedRooms[i]->east));
      }

      if (NULL != parsedRooms[i]->west) {
         printf("West: %s\n", dstrview(parsedRooms[i]->west));
      }

      if (NULL != parsedRooms[i]->objects) {
         printf("Objects: ");
         for (j = 0; parsedRooms[i]->objects[j] != NULL; j++) {
            printf("%s, ", dstrview(parsedRooms[i]->objects[j]));
         }

         printf("\n");
      }

      printf("\n");
   }
}

