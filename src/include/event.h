#ifndef EVENT_H
#define EVENT_H


/* initialize the event handler (MUST be called before first use) */
extern void initEvent();

/* register a new event (note: calling this twice with the same name will
   cause the first to be overwritten) */
extern void registerEvent(char *name, void (*event)(void *));

/* call an event */
extern void event(char *name, void *data);


#endif

