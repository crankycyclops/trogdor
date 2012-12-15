
#ifndef STATE_H
#define STATE_H


#include "trogdor.h"


/* returns 1 if we're running a game and 0 if not */
extern int isInGame();

/* initialize the game's state */
extern void initGame();

/* destroys resources and quits the game */
extern void destroyGame();

/* keeps data consistent between threads */
extern pthread_mutex_t resourceMutex;


#endif

