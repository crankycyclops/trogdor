
#include <stdlib.h>
#include <glib.h>

#include "include/trogdor.h"
#include "include/object.h"
#include "include/room.h"
#include "include/data.h"
#include "include/state.h"
#include "include/player.h"


/* contains the default configuration for all newly initialized players */
Player g_playerConfig = {
   NULL,                /* current location */
   {NULL, NULL, 0, 0}   /* player's inventory */
};

/* an index-by-name list of all players in the game */
GHashTable *g_players;

/* initialize the game's state */
void initGame();

/* creates a new player */
Player *createPlayer(char *name);

/* initializes a player with default starting values */
static void initPlayer(Player *newplayer);

/******************************************************************************/

void initGame() {

   g_players = g_hash_table_new(g_str_hash, g_str_equal);
}

/******************************************************************************/

Player *createPlayer(char *name) {

   Player *newplayer;

   // TODO: if player already exists, return NULL

   newplayer = malloc(sizeof(Player));
   if (NULL == newplayer) {
      PRINT_OUT_OF_MEMORY_ERROR;
   }

   initPlayer(newplayer);
   g_hash_table_insert(g_players, name, newplayer);
   return newplayer;
}

/******************************************************************************/

static void initPlayer(Player *newplayer) {

   newplayer->location = g_hash_table_lookup(rooms, "start");
   newplayer->inventory.list = NULL;
   newplayer->inventory.byName = g_hash_table_new(g_str_hash, g_str_equal);
}

