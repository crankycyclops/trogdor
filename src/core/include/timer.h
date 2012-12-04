#ifndef TIMER_H
#define TIMER_H


typedef struct {

   int id;              /* uniquely identifies jobs in the timer queue */
   int initTime;        /* time when job was registered in the timer queue */
   int interval;        /* timer ticks between each call to job() */
   void (*job)(void *); /* function to execute on each interval */
   void *argument;      /* this argument is passed to job() */

   /* Number of times to execute the job before removing it from the queue.
      This value is decremented on every tick of the clock, and when it
      reaches 0, it's removed from the queue.  If set to -1, it will continue to
      execute indefinitely until it is removed manually. */
   int executions;

} TimedJob;


#ifndef TIMER_C

/* starts the timer */
extern void initTimer();

/* get the current time (how many seconds the game has been running) */
extern unsigned long getTime();

/* Inserts a job into the timer queue.  Takes as input a pointer to the function
   we want to execute, an argument to pass to that function, an interval and
   the number of times we want the job to execute before removing it from the
   queue. */
extern unsigned long registerTimedJob(void (*job)(void *), void *argument,
   int interval, int executions);

/* removes job with the specified id from the timer queue */
extern int deregisterTimedJob(unsigned long id);

#endif


#endif

