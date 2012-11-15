#include <stdio.h>
#include <stdlib.h>
#include <dstring.h>

#include "include/trogdor.h"
#include "include/shell.h"
#include "include/state.h"


/* prints description of an object */
void displayObject(Object *object);

/* processes the posession of an object from the current room */
void takeObject(Object *object);

/* disambiguates in the case where a name refers to more than one object */
Object *clarifyObject(GList *objects, int objectCount);

/******************************************************************************/

void displayObject(Object *object) {

   // TODO: fire event "display object"
   printf("\nYou see a %s.  %s\n", dstrview(object->name),
      dstrview(object->description));
}

/******************************************************************************/

void takeObject(Object *object) {

   int i;

   GList *invHashList = NULL;
   GList *roomHashList = NULL;

   // TODO: fire event "take object"

   inventory = g_list_append(inventory, object);
   location->objectList = g_list_remove(location->objectList, object);

   /* we have to append our object to the list of objects hashed with that
      name's value */
   invHashList = g_hash_table_lookup(inventoryByName, dstrview(object->name));
   roomHashList = g_hash_table_lookup(location->objectByName,
      dstrview(object->name));

   invHashList = g_list_append(invHashList, object);
   g_hash_table_insert(inventoryByName, (char *)dstrview(object->name),
      invHashList);

   roomHashList = g_list_remove(roomHashList, object);
   g_hash_table_insert(location->objectByName, (char *)dstrview(object->name),
      roomHashList);

   /* do the same by name hashing for synonyms */
   for (i = 0; i < object->synonyms->len; i++) {

      dstring_t synonym = g_array_index(object->synonyms, dstring_t, i);

      invHashList = g_hash_table_lookup(inventoryByName, dstrview(synonym));
      roomHashList = g_hash_table_lookup(location->objectByName, dstrview(synonym));

      invHashList = g_list_append(invHashList, object);
      g_hash_table_insert(inventoryByName, (char *)dstrview(synonym), invHashList);

      roomHashList = g_list_remove(roomHashList, object);
      g_hash_table_insert(location->objectByName, (char *)dstrview(synonym), roomHashList);
   }

   printf("You take the %s.\n", dstrview(object->name));
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

