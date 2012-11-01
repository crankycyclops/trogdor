#include <stdio.h>
#include <stdlib.h>

#include "trogdor.h"
#include "data.h"
#include "state.h"
#include "command.h"


void init() {

   /* load assets and game related data */
   initData();

   /* begin in the starting position */
   location = rooms;
}


int main(int argc, char *argv[]) {

   init();

   /* output description of the first room before starting */
   printf("%s\n", location->description);

   /* main game loop */
   while (1) {
      executeCommand();
   }

   return EXIT_SUCCESS;
}

