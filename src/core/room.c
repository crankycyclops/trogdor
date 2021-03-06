
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

   newroom->north = NULL;
   newroom->south = NULL;
   newroom->east = NULL;
   newroom->west = NULL;

   newroom->in = NULL;
   newroom->out = NULL;
   newroom->up = NULL;
   newroom->down = NULL;

   newroom->creatureList = NULL;
   newroom->creatureByName = g_hash_table_new(g_str_hash, g_str_equal);
   newroom->objectList = NULL;
   newroom->objectByName = g_hash_table_new(g_str_hash, g_str_equal);

   /* event handlers */
   newroom->L = NULL;

   /* initialize the room's state */
   newroom->state.players = g_hash_table_new(g_str_hash, g_str_equal);
   newroom->state.visitedByAPlayer = 0;

   newroom->messages = NULL;
   if (initMessages) {
      newroom->messages = createMessages();
   }

   newroom->events = initEventHandlerList();

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

   if (NULL != room->creatureList) {
      g_list_free(room->creatureList);
   }

   if (NULL != room->creatureByName) {
      // TODO: we have to destroy each GList inside objectByName first!
      g_hash_table_destroy(room->creatureByName);
   }

   dstrfree(&room->name);
   dstrfree(&room->title);
   dstrfree(&room->description);

   g_hash_table_destroy(room->state.players);

   if (NULL != room->messages) {
      destroyMessages(room->messages);
   }

   if (room->L != NULL) {
      lua_close(room->L);
   }

   destroyEventHandlerList(room->events);

   free(room);
}

/******************************************************************************/

void setLocation(Player *player, Room *room, int triggerEvents) {

   Room *previous = player->location;

   if (triggerEvents) {
      addEventListener(player->events);
      addEventListener(room->events);
      if (!event("beforeSetLocation", 3, eventArgPlayer(player), eventArgRoom(room),
      eventArgRoom(previous))) {
         return;
      }
   }

   player->location = room;
   displayRoom(player, room, FALSE);

   /* this only tells us that the room was visited and not by who*/
   room->state.visitedByAPlayer = 1;

   /* this tells us who's visited the room */
   g_hash_table_insert(room->state.players, (char *)dstrview(player->name),
      player);

   if (triggerEvents) {
      addEventListener(player->events);
      addEventListener(room->events);
      event("afterSetLocation", 3, eventArgPlayer(player), eventArgRoom(room),
         eventArgRoom(previous));
   }
}

/******************************************************************************/

void displayRoom(Player *player, Room *room, int showLongDescription) {

   GList *creatureList = room->creatureList;
   GList *objectList = room->objectList;

   addEventListener(player->events);
   addEventListener(room->events);
   if (!event("beforeRoomDisplay", 2, eventArgPlayer(player), eventArgRoom(room))) {
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

   addEventListener(player->events);
   addEventListener(room->events);
   event("afterRoomDisplay", 2, eventArgPlayer(player), eventArgRoom(room));
}

/******************************************************************************/

static void describeRoom(Room *room) {

   g_outputString("\n%s\n", dstrview(room->description));
}
