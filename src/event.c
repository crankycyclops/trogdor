
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
static void beforeRoomDisplay(void *data);

/* called after a room is displayed - takes as input the room (type Room *) */
static void afterRoomDisplay(void *data);

/* called before an object is taken - takes as input the object (type Object *) */
static void beforeTakeObject(void *data);

/* called after an object is taken - takes as input the object (type Object *) */
static void afterTakeObject(void *data);

/* called before an object is dropped - takes as input the object (type Object *) */
static void beforeDropObject(void *data);

/* called after an object is dropped - takes as input the object (type Object *) */
static void afterDropObject(void *data);

/* called before an object is described - takes as input the object (type Object *) */
static void beforeDisplayObject(void *data);

/* called after an object is described - takes as input the object (type Object *) */
static void afterDisplayObject(void *data);

/* triggered when the user tries to take an object, but their inventory doesn't
   have enough free weight - data = object we're trying to pick up
   (type Object *) */
static void takeObjectTooHeavy(void *data);

/* called before setting a new location - data = room we're setting (type Room *) */
static void beforeSetLocation(void *data);

/* called after setting a new location - data = room we're setting (type Room *) */
static void afterSetLocation(void *data);


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
   registerEvent("takeObjectTooHeavy",  &takeObjectTooHeavy);
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

   else {
      fprintf(stderr, "WARNING: event %s hasn't been registered.  This is a "
         "bug.\n", name);
   }
}

/******************************************************************************/

static void beforeRoomDisplay(void *data) {

   Room *room = data;

   // TODO
   return;
}

/******************************************************************************/

static void afterRoomDisplay(void *data) {

   Room *room = data;

   // TODO
   return;
}

/******************************************************************************/

static void beforeSetLocation(void *data) {

   Room *room = data;

   // TODO
   return;
}

/******************************************************************************/

static void afterSetLocation(void *data) {

   Room *room = data;

   // TODO
   return;
}

/******************************************************************************/

static void beforeTakeObject(void *data) {

   Object *object = data;

   // TODO
   return;
}

/******************************************************************************/

static void afterTakeObject(void *data) {

   Object *object = data;

   // TODO
   return;
}

/******************************************************************************/

static void beforeDropObject(void *data) {

   Object *object = data;

   // TODO
   return;
}

/******************************************************************************/

static void afterDropObject(void *data) {

   Object *object = data;

   // TODO
   return;
}

/******************************************************************************/

static void beforeDisplayObject(void *data) {

   Object *object = data;

   // TODO
   return;
}

/******************************************************************************/

static void afterDisplayObject(void *data) {

   Object *object = data;

   // TODO
   return;
}

/******************************************************************************/

static void takeObjectTooHeavy(void *data) {

   // TODO
   // TODO: should this call a global script function or a script function
   // unique to the object?  Hmm...  I'm leaning toward global.
   return;
}

