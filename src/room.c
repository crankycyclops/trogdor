
#include <stdio.h>
#include <stdlib.h>
#include <dstring.h>

#include "include/trogdor.h"
#include "include/data.h"
#include "include/state.h"
#include "include/command.h"
#include "include/shell.h"
#include "include/object.h"


/* sets our current location in the game */
void setLocation(Room *room);

/* prints description of a room */
void displayRoom(Room *room);

/******************************************************************************/

void setLocation(Room *room) {

   // TODO: trigger event "set location"
   location = room;
   displayRoom(room);
}

/******************************************************************************/

void displayRoom(Room *room) {

   GList *objectList = room->objectList;

   // TODO: trigger event "display room"

   printf("\n%s\n", dstrview(room->title));
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

