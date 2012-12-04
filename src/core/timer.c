
#define TIMER_C

#include <stdlib.h>
#include <unistd.h>

#include "include/trogdor.h"
#include "include/data.h"
#include "include/state.h"
#include "include/timer.h"


/* starts the timer */
void initTimer();

/* stops the timer and returns the total ellapsed time */
unsigned long destroyTimer();

/* get the current time (how many seconds the game has been running) */
unsigned long getTime();

/* Inserts a job into the timer queue.  Takes as input a pointer to the function
   we want to execute, an argument to pass to that function, an interval and
   the number of times we want the job to execute before removing it from the
   queue. */
unsigned long registerTimedJob(void (*job)(void *), void *argument,
unsigned long start, unsigned long interval, int executions);

/* Removes job with the specified id from the timer queue.  Returns 1 if the
   job was successfully removed and 0 if it was not. */
int deregisterTimedJob(unsigned long id);

/* calls tick() on each interval */
static void *timer(void *threadId);

/* execute the timer's payload */
static void tick();

/* whether or not the timer is active */
static int timerActive = 0;

/* current tick (continually incremented by the ticker) */
static unsigned long gameTime = 0;

/* queue of jobs to execute on each tick of the clock */
static GList *workQueue = NULL;

/* keeps track of the last assigned job id */
static unsigned long lastJobID = 0;

/* use this to synchronize start/stop of the timer */
static pthread_mutex_t timerMutex;

/******************************************************************************/

unsigned long getTime() {

   return gameTime;
}

/******************************************************************************/

void initTimer() {

   pthread_t timerThread;

   gameTime = 0;
   timerActive = 1;

   pthread_mutex_init(&timerMutex, NULL);

   if (pthread_create(&timerThread, NULL, timer, NULL)) {
      g_outputError("Failed to start timer!\n");
      exit(EXIT_FAILURE);
   }
}

/******************************************************************************/

unsigned long destroyTimer() {

   GList *curJob = workQueue;

   /* deactivate the timer */
   pthread_mutex_lock(&timerMutex);
   timerActive = 0;
   pthread_mutex_unlock(&timerMutex);

   /* destroy the work queue and free all memory associated with it */
   while (curJob != NULL) {

      TimedJob *job = (TimedJob *)curJob->data;
      free(job);
      workQueue = g_list_delete_link(workQueue, curJob);

      curJob = curJob->next;
   }

   pthread_mutex_destroy(&timerMutex);
   return gameTime;
}

/******************************************************************************/

unsigned long registerTimedJob(void (*job)(void *), void *argument,
unsigned long start, unsigned long interval, int executions) {

   TimedJob *newjob;

   newjob = malloc(sizeof(TimedJob));
   if (NULL == newjob) {
      PRINT_OUT_OF_MEMORY_ERROR;
   }

   lastJobID++;

   newjob->id = lastJobID;
   newjob->initTime = gameTime;
   newjob->startTime = start;
   newjob->interval = interval;
   newjob->executions = executions;
   newjob->job = job;
   newjob->argument = argument;

   pthread_mutex_lock(&timerMutex);
   workQueue = g_list_append(workQueue, newjob);
   pthread_mutex_unlock(&timerMutex);

   return newjob->id;
}

/******************************************************************************/

int deregisterTimedJob(unsigned long id) {

   GList *curJob = workQueue;

   while (curJob != NULL) {

      if (id == ((TimedJob *)curJob->data)->id) {
         pthread_mutex_lock(&timerMutex);
         TimedJob *job = (TimedJob *)curJob->data;
         free(job);
         workQueue = g_list_delete_link(workQueue, curJob);
         pthread_mutex_unlock(&timerMutex);
         return 1;
      }

      curJob = curJob->next;
   }

   /* job was not found */
   return 0;
}

/******************************************************************************/

static void *timer(void *threadId) {

   while (timerActive) {

      sleep(1);

      pthread_mutex_lock(&timerMutex);
      gameTime++;
      tick();
      pthread_mutex_unlock(&timerMutex);
   }

   return NULL;
}

/******************************************************************************/

static void tick() {

   GList *curJob = workQueue;

   /* we might have destroyed the timer while in the middle of a tick */
   if (!timerActive) {
      return;
   }

   while (curJob != NULL) {

      TimedJob *job = (TimedJob *)curJob->data;

      if (job->executions != 0) {

         if (gameTime - job->initTime >= job->startTime &&
         (gameTime - job->initTime - job->startTime) % job->interval == 0) {

            pthread_mutex_lock(&resourceMutex);
            job->job(job->argument);
            pthread_mutex_unlock(&resourceMutex);

            /* -1 is possible and refers to a job that doesn't expire */
            if (job->executions > 0) {
               job->executions--;
            }
         }
      }

      // job has expired, so remove it
      else {
         pthread_mutex_lock(&timerMutex);
         free(job);
         workQueue = g_list_delete_link(workQueue, curJob);
         pthread_mutex_unlock(&timerMutex);
      }

      curJob = curJob->next;
   }

   return;
}

