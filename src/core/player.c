
#define PLAYER_C

#include <stdlib.h>
#include <dstring.h>

#include "include/trogdor.h"
#include "include/utility.h"
#include "include/room.h"
#include "include/object.h"
#include "include/creature.h"
#include "include/data.h"
#include "include/player.h"


/* creates a new player */
Player *createPlayer(char *name);


/* contains the default configuration for all newly initialized players */
Player g_playerConfig = {
   NULL,                /* player's name */
   NULL,                /* current location */
   {NULL, NULL, 0, 0}   /* player's inventory */
};

/******************************************************************************/

Player *createPlayer(char *name) {

   Player *newplayer;

   // TODO: if player already exists, return NULL

   newplayer = malloc(sizeof(Player));
   if (NULL == newplayer) {
      PRINT_OUT_OF_MEMORY_ERROR;
   }

   newplayer->name = createDstring();
   cstrtodstr(newplayer->name, name);

   /* initialize new player's state */
   newplayer->location = g_hash_table_lookup(g_rooms, "start");
   newplayer->inventory.maxWeight = g_playerConfig.inventory.maxWeight;
   newplayer->inventory.weight = g_playerConfig.inventory.weight;
   newplayer->inventory.list = NULL;
   newplayer->inventory.byName = g_hash_table_new(g_str_hash, g_str_equal);

   g_hash_table_insert(g_players, name, newplayer);
   return newplayer;
}
