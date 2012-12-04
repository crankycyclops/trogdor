
#define TIMER_C

#include <stdlib.h>
#include <unistd.h>

#include "include/trogdor.h"
#include "include/data.h"
#include "include/state.h"
#include "include/timer.h"


/* starts the timer */
void initTimer();

/* get the current time (how many seconds the game has been running) */
int getTime();

/* Inserts a job into the timer queue.  Takes as input a pointer to the function
   we want to execute, an argument to pass to that function, an interval and
   the number of times we want the job to execute before removing it from the
   queue. */
unsigned int registerTimedJob(void (*job)(void *), void *argument, int interval,
   int executions);

/* Removes job with the specified id from the timer queue.  Returns 1 if the
   job was successfully removed and 0 if it was not. */
int deregisterTimedJob(unsigned int id);

/* calls tick() on each interval */
static void *timer(void *threadId);

/* execute the timer's payload */
static void tick();

/* current tick (continually incremented by the ticker) */
static int gameTime = 0;

/* queue of jobs to execute on each tick of the clock */
static GList *workQueue = NULL;

/* keeps track of the last assigned job id */
static unsigned int lastJobID = 0;

/******************************************************************************/

int getTime() {

   return gameTime;
}

/******************************************************************************/

void initTimer() {

   pthread_t timerThread;

   if (pthread_create(&timerThread, NULL, timer, NULL)) {
      g_outputError("Failed to start timer!\n");
      exit(EXIT_FAILURE);
   }
}

/******************************************************************************/

unsigned int registerTimedJob(void (*job)(void *), void *argument, int interval,
int executions) {

   TimedJob *newjob;

   newjob = malloc(sizeof(TimedJob));
   if (NULL == newjob) {
      PRINT_OUT_OF_MEMORY_ERROR;
   }

   lastJobID++;

   newjob->id = lastJobID;
   newjob->initTime = gameTime;
   newjob->interval = interval;
   newjob->executions = executions;
   newjob->job = job;
   newjob->argument = argument;

   workQueue = g_list_append(workQueue, newjob);

   return newjob->id;
}

/******************************************************************************/

int deregisterTimedJob(unsigned int id) {

   GList *curJob = workQueue;

   while (curJob != NULL) {

      if (id == ((TimedJob *)curJob->data)->id) {
         TimedJob *job = (TimedJob *)curJob->data;
         free(job);
         workQueue = g_list_delete_link(workQueue, curJob);
         return 1;
      }

      curJob = curJob->next;
   }

   /* job was not found */
   return 0;
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

   GList *curJob = workQueue;

   while (curJob != NULL) {

      TimedJob *job = (TimedJob *)curJob->data;

      if (job->executions != 0) {

         if (gameTime - job->initTime > 0 &&
         (gameTime - job->initTime) % job->interval == 0) {

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
         free(job);
         workQueue = g_list_delete_link(workQueue, curJob);
      }

      curJob = curJob->next;
   }

   return;
}

