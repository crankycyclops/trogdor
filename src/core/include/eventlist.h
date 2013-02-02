#ifndef EVENTLIST_H
#define EVENTLIST_H


#include <glib.h>


/* A structure that maps event names to handlers */
typedef struct {
   GHashTable *eventHandlers;
   unsigned long nextId;
} EventHandlerList;

#ifndef EVENTLIST_C

/* creates a new list of event handlers */
extern EventHandlerList *initEventHandlerList();

/* destroys a list of event handlers */
extern void destroyEventHandlerList(EventHandlerList *list);

#endif


#endif

