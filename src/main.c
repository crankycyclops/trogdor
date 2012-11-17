#include <stdio.h>
#include <stdlib.h>

#include "include/trogdor.h"
#include "include/room.h"
#include "include/data.h"
#include "include/state.h"
#include "include/command.h"
#include "include/room.h"


int main(int argc, char *argv[]) {

   /* load assets and game related data, and initialize the game's state */
   initData();

   /* output description of the first room before starting */
   setLocation(location);

   /* main game loop */
   while (1) {
      executeCommand();
   }

   return EXIT_SUCCESS;
}

