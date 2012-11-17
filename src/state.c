
#include <glib.h>

#include "include/trogdor.h"
#include "include/object.h"
#include "include/room.h"
#include "include/data.h"
#include "include/state.h"


/* our current location in the game */
Room *location = NULL;

/* list of player-owned objects */
Inventory inventory = {NULL, NULL, 0, 1};

/* initialize the game's state */
void initGame();

/******************************************************************************/

void initGame() {

   location = g_hash_table_lookup(rooms, "start");
   inventory.byName = g_hash_table_new(g_str_hash, g_str_equal);
}

