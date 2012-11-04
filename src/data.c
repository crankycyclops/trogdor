
#include <stdio.h>
#include <stdlib.h>

#include "include/trogdor.h"
#include "include/parsexml.h"

#define GAME_FILE "game.xml"


/* initializes game data/assets such as rooms */
void initData();

/* initializes the rooms */
static void initRooms();

/* frees memory used by game data/assets */
void destroyData();


/* rooms that have been parsed from the XML game file */
RoomParsed **parsedRooms = NULL;

/* game objects that have been parsed from the XML game file */
ObjectParsed **parsedObjects = NULL;

/* all rooms in the game, with the pointer itself referencing "start" */
Room *rooms = NULL;


/* TODO: we should eventually add a parser to do custom room definitions */
void initData() {

   if (!parseGameFile(GAME_FILE)) {
      fprintf(stderr, "failed to parse game file %s\n", GAME_FILE);
      exit(EXIT_FAILURE);
   }

   // TODO
   printParsedRooms();
   printParsedObjects();

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


void printParsedObjects() {

   int i;

   for (i = 0; parsedObjects[i] != NULL; i++) {

      printf("Object %d:\n", i);
      printf("Name: %s\n", dstrview(parsedObjects[i]->name));
      printf("Description: %s\n", dstrview(parsedObjects[i]->description));
   }
}


void printParsedRooms() {

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


void destroyData() {

   // TODO: this doesn't work; only frees "start" room...
   free(rooms);
}

