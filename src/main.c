#include <stdio.h>
#include <stdlib.h>

#include "include/trogdor.h"
#include "include/data.h"
#include "include/state.h"
#include "include/command.h"


int main(int argc, char *argv[]) {

   /* load assets and game related data, and initialize the game's state */
   initData();

   /* output description of the first room before starting */
   // TODO: should be in its own function, displayRoom()
   // TODO: should also display room's name
   printf("%s\n", dstrview(location->description));

   /* main game loop */
   while (1) {
      executeCommand();
   }

   return EXIT_SUCCESS;
}

