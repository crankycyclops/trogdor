
#include <stdlib.h>

#include "include/trogdor.h"
#include "include/utility.h"
#include "include/object.h"
#include "include/room.h"
#include "include/data.h"
#include "include/player.h"
#include "include/timer.h"


/* initialize the game's state */
void initGame();

/* destroys resources and quits the game */
void destroyGame();

/* returns 1 if we're running a game and 0 if not */
int isInGame();


/* keeps data consistent between threads */
pthread_mutex_t resourceMutex;

/* whether or not we're currently running a game */
static int inGame = 0;

/******************************************************************************/

int isInGame() {

   return inGame;
}

/******************************************************************************/

void initGame() {

   pthread_mutex_init(&resourceMutex, NULL);
   g_players = g_hash_table_new(g_str_hash, g_str_equal);

   inGame = 1;
}

/******************************************************************************/

void destroyGame() {

   destroyTimer();
   destroyData();

   inGame = 0;

   pthread_mutex_destroy(&resourceMutex);
   return;
}

