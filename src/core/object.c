
#define OBJECT_C

#include <stdio.h>
#include <stdlib.h>

#include "include/trogdor.h"
#include "include/data.h"
#include "include/object.h"
#include "include/room.h"
#include "include/state.h"
#include "include/player.h"
#include "include/event.h"
#include "include/messages.h"

#define TAKE_OBJECT  1
#define DROP_OBJECT  2


/* Allocates memory for a new game object.  If initMessages is true, we allocate
   memory for the messages structure.  Otherwise, just set it to NULL. */
Object *createObject(int initMessages);

/* frees memory associated with an object */
void destroyObject(Object *object);

/* called everytime a player "sees" the object */
void displayObject(Player *player, Object *object, int showLongDescription);

/* allows a player to describe an object */
static void describeObject(Object *object);

/* processes the posession of an object from the current room */
void takeObject(Player *player, Object *object);

/* let's a player drop the specified object into the current room */
void dropObject(Player *player, Object *object);

/* called internally by takeObject and dropObject:
   action can be TAKE_OBJECT or DROP_OBJECT (defined above) */
static void transferObject(Player *player, Object *object, int action);

/* disambiguates in the case where a name refers to more than one object */
Object *clarifyObject(GList *objects, int objectCount);

/******************************************************************************/

Object *createObject(int initMessages) {

   Object *newobject;

   /* initialize new object structure */
   newobject = malloc(sizeof(Object));
   if (NULL == newobject) {
      PRINT_OUT_OF_MEMORY_ERROR;
   }

   newobject->lua = NULL;

   /* initialize the object's state */
   newobject->state.seenByPlayers = g_hash_table_new(g_str_hash, g_str_equal);
   newobject->state.takenByPlayers = g_hash_table_new(g_str_hash, g_str_equal);
   newobject->state.droppedByPlayers = g_hash_table_new(g_str_hash, g_str_equal);

   newobject->state.seenByPlayer = 0;
   newobject->state.takenByPlayer = 0;
   newobject->state.droppedByPlayer = 0;

   newobject->messages = NULL;
   if (initMessages) {
      newobject->messages = createMessages();
   }

   return newobject;
}

/******************************************************************************/

void destroyObject(Object *object) {

   dstrfree(&object->name);
   dstrfree(&object->description);

   g_array_free(object->synonyms, TRUE);

   g_hash_table_destroy(object->state.seenByPlayers);
   g_hash_table_destroy(object->state.takenByPlayers);
   g_hash_table_destroy(object->state.droppedByPlayers);

   if (object->lua != NULL) {
      lua_close(object->lua);
   }

   free(object);
   return;
}

/******************************************************************************/

void displayObject(Player *player, Object *object, int showLongDescription) {

   if (ALLOW_ACTION != event(player, "beforeDisplayObject", object)) {
      return;
   }

   if (TRUE == showLongDescription ||
   NULL == g_hash_table_lookup(object->state.seenByPlayers,
   (char *)dstrview(player->name))) {

      object->state.seenByPlayer = 1;

      g_hash_table_insert(object->state.seenByPlayers,
         (char *)dstrview(player->name),
         player);

      describeObject(object);
   }

   else {
      g_outputString("\nYou see a %s.\n",
         dstrview(object->name));
   }

   event(player, "afterDisplayObject", object);
}

/******************************************************************************/

static void describeObject(Object *object) {

   g_outputString("\nYou see a %s.  %s\n", dstrview(object->name),
      dstrview(object->description));
}

/******************************************************************************/

void takeObject(Player *player, Object *object) {

   if (ALLOW_ACTION != event(player, "beforeTakeObject", object)) {
      return;
   }

   if (!object->takeable) {
      event(player, "takeObjectUntakeable", object);
      g_outputString("You can't take the %s.\n", dstrview(object->name));
      return;
   }

   else if (0 == player->inventory.maxWeight ||
   player->inventory.weight + object->weight <= player->inventory.maxWeight) {

      transferObject(player, object, TAKE_OBJECT);

      player->inventory.weight += object->weight;

      g_hash_table_insert(object->state.takenByPlayers,
         (char *)dstrview(player->name), player);
      object->state.takenByPlayer = 1;

      g_outputString("You take the %s.\n", dstrview(object->name));
      event(player, "afterTakeObject", object);
   }

   else {
      g_outputString("%s weighs %d and you can only hold %d more.  Try "
         "dropping something first.\n", dstrview(object->name), object->weight,
         player->inventory.maxWeight - player->inventory.weight);
      event(player, "takeObjectTooHeavy", object);
   }
}

/******************************************************************************/

void dropObject(Player *player, Object *object) {

   if (ALLOW_ACTION != event(player, "beforeDropObject", object)) {
      return;
   }

   if (!object->droppable) {
      event(player, "dropObjectUndroppable", object);
      g_outputString("You can't drop the %s.\n", dstrview(object->name));
      return;
   }

   transferObject(player, object, DROP_OBJECT);

   player->inventory.weight -= object->weight;

   g_hash_table_insert(object->state.droppedByPlayers,
      (char *)dstrview(player->name), player);
   object->state.droppedByPlayer = 1;

   g_outputString("You drop the %s.\n", dstrview(object->name));

   event(player, "afterDropObject", object);
}

/******************************************************************************/

static void transferObject(Player *player, Object *object, int action) {

   int i;

   /* lists of objects to transfer ownership to and from */
   GList *srcObjectList  = NULL;
   GList *destObjectList = NULL;

   /* name -> object hash tables for src and dest */
   GHashTable *srcHash  = NULL;
   GHashTable *destHash = NULL;

   /* keeps track of lists of objects indexed by a certain name */
   GList *srcHashList  = NULL;
   GList *destHashList = NULL;

   switch (action) {

      /* we're removing object from the room and adding it to the inventory */
      case TAKE_OBJECT:
         srcObjectList = player->location->objectList;
         destObjectList = player->inventory.list;
         srcHash = player->location->objectByName;
         destHash = player->inventory.byName;
         break;

      /* we're removing object from the inventory and adding it to the room */
      case DROP_OBJECT:
         srcObjectList = player->inventory.list;
         destObjectList = player->location->objectList;
         srcHash = player->inventory.byName;
         destHash = player->location->objectByName;
         break;

      /* WTF? >:( */
      default:
         g_outputError("Unsupported action in ownershipOfObject().  This is a "
            "bug.\n");
         exit(EXIT_FAILURE);
   }

   /* add object to dest and remove it from src */
   destObjectList = g_list_append(destObjectList, object);
   srcObjectList = g_list_remove(srcObjectList, object);

   /* update source and destination by-name indices */
   destHashList = g_hash_table_lookup(destHash, dstrview(object->name));
   srcHashList = g_hash_table_lookup(srcHash, dstrview(object->name));

   /* index object by name in dest */
   destHashList = g_list_append(destHashList, object);
   g_hash_table_insert(destHash, (char *)dstrview(object->name), destHashList);

   /* remove object's index by name from src */
   srcHashList = g_list_remove(srcHashList, object);
   g_hash_table_insert(srcHash, (char *)dstrview(object->name), srcHashList);

   /* update index by name for object's synonyms */
   for (i = 0; i < object->synonyms->len; i++) {

      dstring_t synonym = g_array_index(object->synonyms, dstring_t, i);

      destHashList = g_hash_table_lookup(destHash, dstrview(synonym));
      srcHashList = g_hash_table_lookup(srcHash, dstrview(synonym));

      destHashList = g_list_append(destHashList, object);
      g_hash_table_insert(destHash, (char *)dstrview(synonym), destHashList);

      srcHashList = g_list_remove(srcHashList, object);
      g_hash_table_insert(srcHash, (char *)dstrview(synonym), srcHashList);
   }

   /* we have to update our inventory and room GList pointers! */
   switch (action) {

      case TAKE_OBJECT:
         player->location->objectList = srcObjectList;
         player->inventory.list = destObjectList;
         break;

      case DROP_OBJECT:
         player->inventory.list = srcObjectList;
         player->location->objectList = destObjectList;
         break;

      default:
         break;
   }

   return;
}

/******************************************************************************/

Object *clarifyObject(GList *objects, int objectCount) {

   int        i;
   int        objectFound;
   dstring_t  name;

   Object     *object;
   GList      *curObject = objects;

   /* make sure a function for reading the command was provided by the client */
   if (NULL == g_readCommand) {
      g_outputError("error: client must provide g_readCommand()!\n");
      exit(EXIT_FAILURE);
   }

   g_outputString("Do you mean the ");

   for (i = 0; curObject != NULL; i++) {

      g_outputString("%s", dstrview(((Object *)curObject->data)->name));

      if (i < objectCount - 2) {
         g_outputString(", ");
      }

      else if (i < objectCount - 1) {
         g_outputString(" or the ");
      }

      curObject = g_list_next(curObject);
   }

   g_outputString("?\n");

   /* get an object name from the shell */
   do {
      name = g_readCommand();
   } while (0 == dstrlen(name));

   /* locate the correct object */
   curObject = objects;
   object = NULL;
   objectFound = 0;

   while (curObject != NULL) {

      object = (Object *)curObject->data;

      if (0 == strcmp(dstrview(object->name), dstrview(name))) {
         objectFound = 1;
         break;
      }

      curObject = g_list_next(curObject);
   }

   /* if user entered a non-existent value, try again until we succeed */
   if (!objectFound) {
      g_outputString("There is no %s here!\n", dstrview(name));
      return clarifyObject(objects, objectCount);
   }

   return object;
}

