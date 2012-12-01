#ifndef EVENT_H
#define EVENT_H


/* event() returns whether or not the normal action should be suppressed --
   this is only relevant if the event() is triggered BEFORE the action */
#define SUPPRESS_ACTION  1
#define ALLOW_ACTION     0


#ifndef EVENT_C

/* initialize the event handler (MUST be called before first use) */
extern void initEvent();

/* register a new event (note: calling this twice with the same name will
   cause the first to be overwritten) */
extern void registerEvent(char *name, int (*event)(void *));

/* call an event */
extern int event(char *name, void *data);

#endif


#endif

