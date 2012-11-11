
#include <glib.h>

#include "include/trogdor.h"
#include "include/data.h"


/* our current location in the game */
Room *location = NULL;

/* a doubly linked list of all items in the user's inventory */
GList *inventory = NULL;

/* hash of object names and synonyms for quick lookup by name */
GHashTable *inventoryByName = NULL;

/* initialize the game's state */
void initGame();

/******************************************************************************/

void initGame() {

   location = g_hash_table_lookup(rooms, "start");
   inventoryByName = g_hash_table_new(g_str_hash, g_str_equal);
}

