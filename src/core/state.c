
#include <stdlib.h>

#include "include/trogdor.h"
#include "include/utility.h"
#include "include/object.h"
#include "include/room.h"
#include "include/data.h"
#include "include/state.h"
#include "include/player.h"
#include "include/timer.h"


/* contains the default configuration for all newly initialized players */
Player g_playerConfig = {
   NULL,                /* player's name */
   NULL,                /* current location */
   {NULL, NULL, 0, 0}   /* player's inventory */
};

/* an index-by-name list of all players in the game */
GHashTable *g_players;

/* initialize the game's state */
void initGame();

/* destroys resources and quits the game */
void destroyGame();

/* creates a new player */
Player *createPlayer(char *name);

/* initializes a player with default starting values */
static void initPlayer(Player *newplayer);

/* keeps data consistent between threads */
pthread_mutex_t resourceMutex;

/******************************************************************************/

void initGame() {

   pthread_mutex_init(&resourceMutex, NULL);
   g_players = g_hash_table_new(g_str_hash, g_str_equal);
}

/******************************************************************************/

void destroyGame() {

   destroyTimer();
   destroyData();

   /* close any leftover threads (TODO: is this necessary?) */
   pthread_mutex_destroy(&resourceMutex);
   pthread_exit(NULL);

   g_outputString("Goodbye!\n");
   exit(EXIT_SUCCESS);
}

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

   initPlayer(newplayer);
   g_hash_table_insert(g_players, name, newplayer);

   return newplayer;
}

/******************************************************************************/

static void initPlayer(Player *newplayer) {

   newplayer->location = g_hash_table_lookup(rooms, "start");

   newplayer->inventory.maxWeight = g_playerConfig.inventory.maxWeight;
   newplayer->inventory.weight = g_playerConfig.inventory.weight;
   newplayer->inventory.list = NULL;
   newplayer->inventory.byName = g_hash_table_new(g_str_hash, g_str_equal);
}

