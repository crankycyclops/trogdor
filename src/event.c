
#define EVENT_C

#include <glib.h>

#include "include/object.h"
#include "include/room.h"
#include "include/state.h"
#include "include/event.h"


/* initialize the event handler (MUST be called before first use) */
void initEvent();

/* register a new event (note: calling this twice with the same name will
   cause the first to be overwritten) */
void registerEvent(char *name, int (*event)(void *));

/* call an event */
int event(char *name, void *data);


/******************\
 * List of events *
\******************/

/* called before a room is displayed - takes as input the room (type Room *) */
static int beforeRoomDisplay(void *data);

/* called after a room is displayed - takes as input the room (type Room *) */
static int afterRoomDisplay(void *data);

/* called before an object is taken - takes as input the object (type Object *) */
static int beforeTakeObject(void *data);

/* called after an object is taken - takes as input the object (type Object *) */
static int afterTakeObject(void *data);

/* called before an object is dropped - takes as input the object (type Object *) */
static int beforeDropObject(void *data);

/* called after an object is dropped - takes as input the object (type Object *) */
static int afterDropObject(void *data);

/* called before an object is described - takes as input the object (type Object *) */
static int beforeDisplayObject(void *data);

/* called after an object is described - takes as input the object (type Object *) */
static int afterDisplayObject(void *data);

/* triggered when the user tries to take an object, but their inventory doesn't
   have enough free weight - data = object we're trying to pick up
   (type Object *) */
static int takeObjectTooHeavy(void *data);

/* called before setting a new location - data = room we're setting (type Room *) */
static int beforeSetLocation(void *data);

/* called after setting a new location - data = room we're setting (type Room *) */
static int afterSetLocation(void *data);


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

void registerEvent(char *name, int (*event)(void *)) {

   g_hash_table_insert(events, name, event);
}

/******************************************************************************/

int event(char *name, void *data) {

   int (*event)(void *) = g_hash_table_lookup(events, name);

   if (NULL != event) {
      return event(data);
   }

   else {
      fprintf(stderr, "WARNING: event %s hasn't been registered.  This is a "
         "bug.\n", name);
      return ALLOW_ACTION;
   }
}

/******************************************************************************/

static int beforeRoomDisplay(void *data) {

   Room *room = data;

   // TODO
   return ALLOW_ACTION;
}

/******************************************************************************/

static int afterRoomDisplay(void *data) {

   Room *room = data;

   // TODO
   return ALLOW_ACTION;
}

/******************************************************************************/

static int beforeSetLocation(void *data) {

   Room *room = data;

   // TODO
   return ALLOW_ACTION;
}

/******************************************************************************/

static int afterSetLocation(void *data) {

   Room *room = data;

   // TODO
   return ALLOW_ACTION;
}

/******************************************************************************/

static int beforeTakeObject(void *data) {

   Object *object = data;

   // TODO
   return ALLOW_ACTION;
}

/******************************************************************************/

static int afterTakeObject(void *data) {

   Object *object = data;

   // TODO
   return ALLOW_ACTION;
}

/******************************************************************************/

static int beforeDropObject(void *data) {

   Object *object = data;

   // TODO
   return ALLOW_ACTION;
}

/******************************************************************************/

static int afterDropObject(void *data) {

   Object *object = data;

   // TODO
   return ALLOW_ACTION;
}

/******************************************************************************/

static int beforeDisplayObject(void *data) {

   Object *object = data;

   // TODO
   return ALLOW_ACTION;
}

/******************************************************************************/

static int afterDisplayObject(void *data) {

   Object *object = data;

   // TODO
   return ALLOW_ACTION;
}

/******************************************************************************/

static int takeObjectTooHeavy(void *data) {

   // TODO
   // TODO: should this call a global script function or a script function
   // unique to the object?  Hmm...  I'm leaning toward global.
   return ALLOW_ACTION;
}

