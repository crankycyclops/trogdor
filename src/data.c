
#include <stdio.h>
#include <stdlib.h>

#include "trogdor.h"


/* initializes game data/assets such as rooms */
void initData();

/* frees memory used by game data/assets */
void destroyData();


Room *rooms = NULL;


/* TODO: we should eventually add a parser to do custom room definitions */
void initData() {

   Room *start;

   start = (Room *)malloc(sizeof(Room));

   start->description = "This is such a cool room!";
   start->north = NULL;
   start->south = NULL;
   start->east  = NULL;
   start->west  = NULL;

   rooms = start;

   return;
}


void destroyData() {

   free(rooms);
}

