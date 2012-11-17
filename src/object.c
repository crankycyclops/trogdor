#include <stdio.h>
#include <stdlib.h>
#include <glib.h>
#include <dstring.h>

#include "include/trogdor.h"
#include "include/object.h"
#include "include/room.h"
#include "include/shell.h"
#include "include/state.h"

#define OBJECT_C

#define TAKE_OBJECT  1
#define DROP_OBJECT  2


/* prints description of an object */
void displayObject(Object *object);

/* processes the posession of an object from the current room */
void takeObject(Object *object);

/* drops the specified object into the current room */
void dropObject(Object *object);

/* called internally by takeObject and dropObject:
   action can be TAKE_OBJECT or DROP_OBJECT (defined above) */
static void transferObject(Object *object, int action);

/* disambiguates in the case where a name refers to more than one object */
Object *clarifyObject(GList *objects, int objectCount);

/******************************************************************************/

void displayObject(Object *object) {

   event("beforeDisplayObject", object);

   printf("\nYou see a %s.  %s\n", dstrview(object->name),
      dstrview(object->description));

   event("afterDisplayObject", object);
}

/******************************************************************************/

void takeObject(Object *object) {

   event("beforeTakeObject", object);

   if (0 == inventory.maxWeight ||
   inventory.weight + object->weight <= inventory.maxWeight) {
      transferObject(object, TAKE_OBJECT);
      inventory.weight += object->weight;
      printf("You take the %s.\n", dstrview(object->name));
      event("afterTakeObject", object);
   }

   else {
      printf("%s is too heavy.  Try dropping something first.\n",
         dstrview(object->name));
      event("takeObjectTooHeavy", object);
   }
}

/******************************************************************************/

void dropObject(Object *object) {

   event("beforeDropObject", object);

   transferObject(object, DROP_OBJECT);
   inventory.weight -= object->weight;
   printf("You drop the %s.\n", dstrview(object->name));

   event("afterDropObject", object);
}

/******************************************************************************/

static void transferObject(Object *object, int action) {

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
         srcObjectList = location->objectList;
         destObjectList = inventory.list;
         srcHash = location->objectByName;
         destHash = inventory.byName;
         break;

      /* we're removing object from the inventory and adding it to the room */
      case DROP_OBJECT:
         srcObjectList = inventory.list;
         destObjectList = location->objectList;
         srcHash = inventory.byName;
         destHash = location->objectByName;
         break;

      /* WTF? >:( */
      default:
         fprintf(stderr, "Unsupported action in ownershipOfObject().  "
            "This is a bug.\n");
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
         location->objectList = srcObjectList;
         inventory.list = destObjectList;
         break;

      case DROP_OBJECT:
         inventory.list = srcObjectList;
         location->objectList = destObjectList;
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

   printf ("Do you mean the ");

   for (i = 0; curObject != NULL; i++) {

      printf("%s", dstrview(((Object *)curObject->data)->name));

      if (i < objectCount - 2) {
         printf(", ");
      }

      else if (i < objectCount - 1) {
         printf(" or the ");
      }

      curObject = g_list_next(curObject);
   }

   printf("?\n");

   /* get an object name from the shell */
   do {
      name = readCommand();
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
      printf("There is no %s here!\n", dstrview(name));
      return clarifyObject(objects, objectCount);
   }

   return object;
}

