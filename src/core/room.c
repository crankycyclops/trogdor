
#define ROOM_C

#include <stdio.h>
#include <stdlib.h>

#include "include/trogdor.h"
#include "include/object.h"
#include "include/room.h"
#include "include/creature.h"
#include "include/data.h"
#include "include/state.h"
#include "include/player.h"
#include "include/command.h"
#include "include/event.h"
#include "include/messages.h"


/* Allocates memory for a room.  If initMessages is true, we allocate memory
   for the messages structure.  Otherwise, just set it to NULL. */
Room *createRoom(int initMessages);

/* frees memory associated with a room */
void destroyRoom(Room *room);

/* sets a player's current location in the game */
void setLocation(Player *player, Room *room, int triggerEvents);

/* prints description of a room (only the title once already visited) */
void displayRoom(Player *player, Room *room, int showLongDescription);

/* prints the long description of a room */
static void describeRoom(Room *room);

/******************************************************************************/

Room *createRoom(int initMessages) {

   Room *newroom;

   newroom = malloc(sizeof(Room));
   if (NULL == newroom) {
      PRINT_OUT_OF_MEMORY_ERROR;
   }

   /* initialize the room's state */
   newroom->state.players = g_hash_table_new(g_str_hash, g_str_equal);
   newroom->state.visitedByAPlayer = 0;

   newroom->messages = NULL;
   if (initMessages) {
      newroom->messages = createMessages();
   }

   return newroom;
}

/******************************************************************************/

void destroyRoom(Room *room) {

   int i;

   if (NULL != room->objectList) {
      g_list_free(room->objectList);
   }

   if (NULL != room->objectByName) {
      // TODO: we have to destroy each GList inside objectByName first!
      g_hash_table_destroy(room->objectByName);
   }

   dstrfree(&room->name);
   dstrfree(&room->title);
   dstrfree(&room->description);

   g_hash_table_destroy(room->state.players);

   if (NULL != room->messages) {
      destroyMessages(room->messages);
   }

   free(room);
}

/******************************************************************************/

void setLocation(Player *player, Room *room, int triggerEvents) {

   if (ALLOW_ACTION != event(player, "beforeSetLocation", room)) {
      return;
   }

   player->location = room;
   displayRoom(player, room, FALSE);

   /* this only tells us that the room was visited and not by who*/
   room->state.visitedByAPlayer = 1;

   /* this tells us who's visited the room */
   g_hash_table_insert(room->state.players, (char *)dstrview(player->name),
      player);

   event(player, "afterSetLocation", room);
}

/******************************************************************************/

void displayRoom(Player *player, Room *room, int showLongDescription) {

   GList *creatureList = room->creatureList;
   GList *objectList = room->objectList;

   if (ALLOW_ACTION != event(player, "beforeRoomDisplay", room)) {
      return;
   }

   g_outputString("\n%s\n", dstrview(room->title));
   if (showLongDescription ||
   NULL == g_hash_table_lookup(room->state.players,
   (char *)dstrview(player->name))) {
      describeRoom(room);
   }

   /* display objects in the room if they haven't already been seen */
   while (objectList != NULL) {
      displayObject(player, (Object *)objectList->data, FALSE);
      objectList = g_list_next(objectList);
   }

   /* display creatures in the room if they haven't already been seen */
   while (creatureList != NULL) {
      displayCreature(player, (Creature *)creatureList->data, FALSE);
      creatureList = g_list_next(creatureList);
   }

   event(player, "afterRoomDisplay", room);
}

/******************************************************************************/

static void describeRoom(Room *room) {

   g_outputString("\n%s\n", dstrview(room->description));
}
