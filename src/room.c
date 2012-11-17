
#include <stdio.h>
#include <stdlib.h>
#include <dstring.h>

#include "include/trogdor.h"
#include "include/object.h"
#include "include/room.h"
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

   // TODO: trigger event "before set location"

   location = room;
   displayRoom(room);

   // TODO: trigger event "after set location"
}

/******************************************************************************/

void displayRoom(Room *room) {

   GList *objectList = room->objectList;

   // TODO: trigger event "before display room"

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

   // TODO: trigger event "after display room"
}

