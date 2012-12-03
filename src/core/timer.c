
#include <stdlib.h>
#include <unistd.h>

#include "include/trogdor.h"
#include "include/data.h"
#include "include/state.h"


/* starts the timer */
void initTimer();

/* get the current time (how many seconds the game has been running) */
int getTime();

/* calls tick() on each interval */
static void *timer(void *threadId);

/* execute the timer's payload */
static void tick();

/* current tick (continually incremented by the ticker) */
static int gameTime = 0;

/******************************************************************************/

int getTime() {

   return gameTime;
}

/******************************************************************************/

void initTimer() {

   pthread_t timerThread;

   if (pthread_create(&timerThread, NULL, timer, TIMER_THREAD_ID)) {
      g_outputError("Failed to start timer!\n");
      exit(EXIT_FAILURE);
   }
}

/******************************************************************************/

static void *timer(void *threadId) {

   while (1) {
      gameTime++;
      sleep(1);
      tick();
   }

   return NULL;
}

/******************************************************************************/

static void tick() {

   // NOTE: only do locking if payload actually does anything...
   pthread_mutex_lock(&resourceMutex);
   // TODO: tick payload
   pthread_mutex_unlock(&resourceMutex);

   return;
}

