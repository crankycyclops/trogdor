
#include <stdio.h>
#include <stdlib.h>
#include <dstring.h>

#include "include/trogdor.h"

/* prints description of a room */
void displayRoom(Room *room);

/* prints description of an object */
void displayObject(Object *object);

/******************************************************************************/

void displayRoom(Room *room) {

   GList *objectList = room->objectList;

   printf("\n%s\n", dstrview(room->description));

   /* display objects in the room if they haven't already been seen */
   while (objectList != NULL) {

      if (!((Object *)objectList->data)->seen) {
         displayObject((Object *)objectList->data);
         ((Object *)objectList->data)->seen = 1;
      }

      else {
         printf("\nYou see a %s.\n",
            dstrview(((Object *)objectList->data)->name));
      }

      objectList = g_list_next(objectList);
   }
}

/******************************************************************************/

void displayObject(Object *object) {

   printf("\nYou see a %s.  %s\n", dstrview(object->name),
      dstrview(object->description));
}

