
#include <glib.h>

#include "include/trogdor.h"
#include "include/data.h"


/* our current location in the game */
Room *location = NULL;

/* initialize the game's state */
void initGame();

/******************************************************************************/

void initGame() {

   location = g_hash_table_lookup(rooms, "start");
}

