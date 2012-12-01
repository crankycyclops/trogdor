#include <stdio.h>
#include <stdlib.h>

#include "../core/include/trogdor.h"
#include "../core/include/room.h"
#include "../core/include/data.h"
#include "../core/include/state.h"
#include "../core/include/room.h"
#include "../core/include/event.h"

#include "include/shell.h"


int main(int argc, char *argv[]) {

   /* register our shell with the core */
   g_readCommand = &readCommandFromStdin;

   /* initialize our event handler */
   initEvent();

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

