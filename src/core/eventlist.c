
#define EVENTLIST_C

#include <stdlib.h>
#include <glib.h>

#include "include/eventlist.h"
#include "include/event.h"


/* creates a new list of event handlers */
EventHandlerList *initEventHandlerList();

/* destroys a list of event handlers */
void destroyEventHandlerList(EventHandlerList *list);

/******************************************************************************/

EventHandlerList *initEventHandlerList() {

   EventHandlerList *list = malloc(sizeof(EventHandler));

   if (NULL == list) {
      PRINT_OUT_OF_MEMORY_ERROR;
   }

   list->eventHandlers = g_hash_table_new(g_str_hash, g_str_equal);
   list->nextId = 0;

   return list;
}

/******************************************************************************/

void destroyEventHandlerList(EventHandlerList *list) {

   GList *events = g_hash_table_get_values(list->eventHandlers);
   GList *next = events;

   while (next != NULL) {
      g_list_free(next->data);
      next = next->next;
   }

   g_list_free(events);
   g_hash_table_destroy(list->eventHandlers);
   free(list);
}

