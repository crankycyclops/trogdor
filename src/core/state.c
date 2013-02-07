
#include <stdlib.h>
#include <time.h>

#include "include/trogdor.h"
#include "include/utility.h"
#include "include/object.h"
#include "include/room.h"
#include "include/data.h"
#include "include/player.h"
#include "include/timer.h"
#include "include/event.h"
#include "include/eventhandlers.h"


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

   // TODO: should event handlers go in their own function?
   /* trigger auto-attack against player by any creatures in the room that have
      been configured to do so */
   addGlobalEventHandler("afterSetLocation", &eventHandlerAutoAttack);

   pthread_mutex_init(&resourceMutex, NULL);
   g_players = g_hash_table_new(g_str_hash, g_str_equal);

   inGame = 1;

   /* seed the random number generator (for various stuff) */
   srand(time(NULL));
}

/******************************************************************************/

void destroyGame() {

   destroyTimer();
   destroyData();

   inGame = 0;

   pthread_mutex_destroy(&resourceMutex);
   return;
}

