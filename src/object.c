#include <stdio.h>
#include <stdlib.h>
#include <dstring.h>

#include "include/trogdor.h"
#include "include/shell.h"
#include "include/state.h"

#define TAKE_OBJECT  1
#define DROP_OBJECT  2


/* prints description of an object */
void displayObject(Object *object);

/* called internally by takeObject and dropObject:
   action can be TAKE_OBJECT or DROP_OBJECT (defined above) */
static void ownershipOfObject(Object *object, int action);

/* processes the posession of an object from the current room */
void takeObject(Object *object);

/* drops the specified object into the current room */
void dropObject(Object *object);

/* disambiguates in the case where a name refers to more than one object */
Object *clarifyObject(GList *objects, int objectCount);

/******************************************************************************/

void displayObject(Object *object) {

   // TODO: fire event "before display object"

   printf("\nYou see a %s.  %s\n", dstrview(object->name),
      dstrview(object->description));

   // TODO: fire event "after display object"
}

/******************************************************************************/

static void ownershipOfObject(Object *object, int action) {

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
         destObjectList = inventory;
         srcHash = location->objectByName;
         destHash = inventoryByName;
         break;

      /* we're removing object from the inventory and adding it to the room */
      case DROP_OBJECT:
         srcObjectList = inventory;
         destObjectList = location->objectList;
         srcHash = inventoryByName;
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

      /*  */
      case TAKE_OBJECT:
         location->objectList = srcObjectList;
         inventory = destObjectList;
         break;

      /* we're removing object from the inventory and adding it to the room */
      case DROP_OBJECT:
         inventory = srcObjectList;
         location->objectList = destObjectList;
         break;

      default:
         break;
   }

   return;
}

/******************************************************************************/

void takeObject(Object *object) {

   // TODO: fire event "before take object"

   ownershipOfObject(object, TAKE_OBJECT);
   printf("You take the %s.\n", dstrview(object->name));

   // TODO: fire event "after take object"
   return;
}

/******************************************************************************/

void dropObject(Object *object) {

   // TODO
   printf("STUB: drop object %s\n", dstrview(object->name));
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

