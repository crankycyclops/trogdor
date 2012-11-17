
#include <glib.h>

#include "include/object.h"
#include "include/room.h"
#include "include/state.h"


/* initialize the event handler (MUST be called before first use) */
void initEvent();

/* register a new event (note: calling this twice with the same name will
   cause the first to be overwritten) */
void registerEvent(char *name, void (*event)(void *));

/* call an event */
void event(char *name, void *data);


/******************\
 * List of events *
\******************/

/* called before a room is displayed - takes as input the room (type Room *) */
void beforeRoomDisplay(void *data);

/* called after a room is displayed - takes as input the room (type Room *) */
void afterRoomDisplay(void *data);

/* called before an object is taken - takes as input the object (type Object *) */
void beforeTakeObject(void *data);

/* called after an object is taken - takes as input the object (type Object *) */
void afterTakeObject(void *data);

/* called before an object is dropped - takes as input the object (type Object *) */
void beforeDropObject(void *data);

/* called after an object is dropped - takes as input the object (type Object *) */
void afterDropObject(void *data);

/* called before an object is described - takes as input the object (type Object *) */
void beforeDisplayObject(void *data);

/* called after an object is described - takes as input the object (type Object *) */
void afterDisplayObject(void *data);

/* called before setting a new location - data = room we're setting (type Room *) */
void beforeSetLocation(void *data);

/* called after setting a new location - data = room we're setting (type Room *) */
void afterSetLocation(void *data);


/* maintains a mapping of event names to functions */
static GHashTable *events = NULL;


/******************************************************************************/

void initEvent() {

   events = g_hash_table_new(g_str_hash, g_str_equal);

   registerEvent("beforeRoomDisplay",   &beforeRoomDisplay);
   registerEvent("afterRoomDisplay",    &afterRoomDisplay);
   registerEvent("beforeSetLocation",   &beforeSetLocation);
   registerEvent("afterSetLocation",    &afterSetLocation);
   registerEvent("beforeTakeObject",    &beforeTakeObject);
   registerEvent("afterTakeObject",     &afterTakeObject);
   registerEvent("beforeDropObject",    &beforeDropObject);
   registerEvent("afterDropObject",     &afterDropObject);
   registerEvent("beforeDisplayObject", &beforeDisplayObject);
   registerEvent("afterDisplayObject",  &afterDisplayObject);
}

/******************************************************************************/

void registerEvent(char *name, void (*event)(void *)) {

   g_hash_table_insert(events, name, event);
}

/******************************************************************************/

void event(char *name, void *data) {

   void (*event)(void *) = g_hash_table_lookup(events, name);

   if (NULL != event) {
      event(data);
   }
}

/******************************************************************************/

void beforeRoomDisplay(void *data) {

   Room *room = data;

   // TODO
   return;
}

/******************************************************************************/

void afterRoomDisplay(void *data) {

   Room *room = data;

   // TODO
   return;
}

/******************************************************************************/

void beforeSetLocation(void *data) {

   Room *room = data;

   // TODO
   return;
}

/******************************************************************************/

void afterSetLocation(void *data) {

   Room *room = data;

   // TODO
   return;
}

/******************************************************************************/

void beforeTakeObject(void *data) {

   Object *object = data;

   // TODO
   return;
}

/******************************************************************************/

void afterTakeObject(void *data) {

   Object *object = data;

   // TODO
   return;
}

/******************************************************************************/

void beforeDropObject(void *data) {

   Object *object = data;

   // TODO
   return;
}

/******************************************************************************/

void afterDropObject(void *data) {

   Object *object = data;

   // TODO
   return;
}

/******************************************************************************/

void beforeDisplayObject(void *data) {

   Object *object = data;

   // TODO
   return;
}

/******************************************************************************/

void afterDisplayObject(void *data) {

   Object *object = data;

   // TODO
   return;
}

