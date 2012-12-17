#include <stdio.h>
#include <stdlib.h>

#include "include/trogdor.h"
#include "include/object.h"
#include "include/room.h"
#include "include/creature.h"
#include "include/data.h"
#include "include/state.h"
#include "include/construct.h"
#include "include/script.h"
#include "include/event.h"
#include "include/messages.h"


/* entry point for parsing the game file */
int parseGame();

/* initialize game objects in a room from parsed data */
static void initObjects();

/* builds creature structures */
static void initCreatures();

/* initializes the rooms */
static void initRooms();

/* called by initRooms(); builds the structure for a room */
static Room *initRoom(RoomParsed *roomParsed);

/* creates individual creature structures; called by initCreatures() */
static Creature *initCreature(CreatureParsed *creatureParsed);

/* called by initObjects(); builds the structure for an object */
static Object *initObject(ObjectParsed *objectParsed);

/* initializes a room's, object's or creature's Lua state */
static lua_State *initLuaState(GArray *scriptList);

/* connect rooms so that users can navigate north, south, etc. */
static void connectRooms();

/* adds an object to a linked list and a hash table (by name and synonym) */
static GList *addObject(GList *list, GHashTable *byName, dstring_t name);

/* prints parsed data for a room */
static void printParsedRoom(RoomParsed *room);

/* prints parsed data for an object */
static void printParsedObject(ObjectParsed *object);

/* initializes the parser */
static void initParser();

/* frees memory associated with the parser */
static void destroyParser();

/* a lookup table for game objects being parsed */
GHashTable *objectParsedTable = NULL;

/* a lookup table for rooms being parsed */
GHashTable *roomParsedTable = NULL;

/* a lookup table for creatures being parsed */
GHashTable *creatureParsedTable = NULL;

/******************************************************************************/

int parseGame(const char *filename) {

   initParser();

   if (!parseGameFile(filename)) {
      g_outputError("failed to parse game file %s\n", filename);
      return 0;
   }

   /* make sure a room named "start" exists */
   if (NULL == g_hash_table_lookup(roomParsedTable, "start")) {
      g_outputError("room 'start' must be defined\n");
      return 0;
   }

   initObjects();
   initCreatures();
   initRooms();

   /* make sure we didn't have any issues parsing Lua scripts */
   if (g_scriptError) {
      return 0;
   }

   destroyParser();
   return 1;
}

/******************************************************************************/

static void initParser() {

   objectParsedTable   = g_hash_table_new(g_str_hash, g_str_equal);
   roomParsedTable     = g_hash_table_new(g_str_hash, g_str_equal);
   creatureParsedTable = g_hash_table_new(g_str_hash, g_str_equal);
}

/******************************************************************************/

static void destroyParser() {

   g_hash_table_destroy(objectParsedTable);
   g_hash_table_destroy(roomParsedTable);
   g_hash_table_destroy(creatureParsedTable);
}

/******************************************************************************/

static void initCreatures() {

   GList *creatures = g_hash_table_get_values(creatureParsedTable);
   GList *next = creatures;

   while (NULL != next) {
      CreatureParsed *creatureDef = (CreatureParsed *)next->data;
      Creature *creature = initCreature(creatureDef);
      g_hash_table_insert(g_creatures, (char *)dstrview(creature->name),
         creature);
      next = g_list_next(next);
   }
}

/******************************************************************************/

static Creature *initCreature(CreatureParsed *creatureParsed) {

   int i;
   Creature *creature = createCreature(FALSE);
   GArray *objects = creatureParsed->objects;

   creature->name = creatureParsed->name;
   creature->title = creatureParsed->title;
   creature->description = creatureParsed->description;
   creature->messages = creatureParsed->messages;

   creature->lua = initLuaState(creatureParsed->scripts);

   /* add objects to the creature's inventory */
   for (i = 0; i < objects->len; i++) {
      creature->inventory.list = addObject(creature->inventory.list,
         creature->inventory.byName, g_array_index(objects, dstring_t, i));
   }

   return creature;
}

/******************************************************************************/

static void initRooms() {

   Room *room;  /* actual room object */
   GList *parsedRoomList = g_hash_table_get_values(roomParsedTable);
   GList *curParsedRoom = parsedRoomList;

   /* iterate through each value in the rooms parsed table */
   while (NULL != curParsedRoom) {

      /* build the room structure and index it by name */
      room = initRoom((RoomParsed *)curParsedRoom->data);
      g_hash_table_insert(g_rooms, (char *)dstrview(room->name), room);

      curParsedRoom = curParsedRoom->next;
   }

   connectRooms();

   g_list_free(parsedRoomList);
   return;
}

/******************************************************************************/

static Room *initRoom(RoomParsed *roomParsed) {

   int i;

   GArray *creatureNames = roomParsed->creatures; /* creatures in the room */
   GArray *objectNames = roomParsed->objects;     /* objects in the room */
   Room *room = createRoom(FALSE);                /* actual room object */

   /* list of objects referenced by a name */
   GList *synonymList;

   /* don't free these dstring_t objects when we free the parsed rooms table! */
   room->name = roomParsed->name;
   room->title = roomParsed->title;
   room->description = roomParsed->description;

   /* rooms will be connected later by another function */
   room->north = NULL;
   room->south = NULL;
   room->east  = NULL;
   room->west  = NULL;
   room->in    = NULL;
   room->out   = NULL;
   room->up    = NULL;
   room->down  = NULL;

   room->messages = roomParsed->messages;
   room->creatureList = NULL;
   room->objectList = NULL;
   room->objectByName = g_hash_table_new(g_str_hash, g_str_equal);

   /* add creatures to the room */
   for (i = 0; i < creatureNames->len; i++) {

      Creature *creature = g_hash_table_lookup(g_creatures,
         dstrview(g_array_index(creatureNames, dstring_t, i)));

      room->creatureList = g_list_append(room->creatureList, creature);
   }

   /* add objects to the room */
   for (i = 0; i < objectNames->len; i++) {
      room->objectList = addObject(room->objectList, room->objectByName,
         g_array_index(objectNames, dstring_t, i));
   }

   return room;
}

/******************************************************************************/

static void initObjects() {

   GList *objects = g_hash_table_get_values(objectParsedTable);
   GList *nextObj = objects;


   while (NULL != nextObj) {
      Object *object = initObject((ObjectParsed *)nextObj->data);
      g_hash_table_insert(g_objects, (char *)dstrview(object->name), object);
      nextObj = nextObj->next;
   }

   return;
}

/******************************************************************************/

static Object *initObject(ObjectParsed *objectParsed) {

   Object *object = createObject(FALSE);

   object->name = objectParsed->name;
   object->description = objectParsed->description;
   object->weight = atoi(dstrview(objectParsed->weight));
   object->takeable = atoi(dstrview(objectParsed->takeable));
   object->droppable = atoi(dstrview(objectParsed->droppable));
   object->messages = objectParsed->messages;

   object->synonyms = objectParsed->synonyms;

   object->lua = initLuaState(objectParsed->scripts);

   return object;
}

/******************************************************************************/

static lua_State *initLuaState(GArray *scriptList) {

   int i;
   lua_State *L = NULL;

   for (i = 0; i < scriptList->len; i++) {

      /* only initialize lua state if we have something to execute */
      if (NULL == L) {
         L = luaL_newstate();
         initLua(L);
      }

      #define SCRIPT_FILE (char *)dstrview(g_array_index(scriptList, dstring_t, i))
      if (loadScript(L, SCRIPT_FILE)) {
         primeLua(L);
      }
      #undef SCRIPT_FILE
   }

   return L;
}

/******************************************************************************/

static void connectRooms() {

   GList *roomNames = g_hash_table_get_keys(g_rooms);
   GList *currentRoomName = roomNames;

   while (currentRoomName != NULL) {

      Room *room = g_hash_table_lookup(g_rooms, currentRoomName->data);
      Room *connected = NULL;
      RoomParsed *roomDefinition = g_hash_table_lookup(roomParsedTable,
         currentRoomName->data);

      if (NULL != roomDefinition->north) {

         connected = g_hash_table_lookup(g_rooms, dstrview(roomDefinition->north));

         /* game file specified a room that wasn't defined! */ 
         if (NULL == connected) {
            g_outputError("error: room '%s' connects to room '%s', but "
               "room '%s' doesn't exist!\n", dstrview(room->name),
               dstrview(roomDefinition->north), dstrview(roomDefinition->north));
            exit(EXIT_FAILURE);
         }

         room->north = connected;
      }

      if (NULL != roomDefinition->south) {

         connected = g_hash_table_lookup(g_rooms, dstrview(roomDefinition->south));

         if (NULL == connected) {
            g_outputError("error: room '%s' connects to room '%s', but "
               "room '%s' doesn't exist!\n", dstrview(room->name),
               dstrview(roomDefinition->south), dstrview(roomDefinition->south));
            exit(EXIT_FAILURE);
         }

         room->south = connected;
      }

      if (NULL != roomDefinition->east) {

         connected = g_hash_table_lookup(g_rooms, dstrview(roomDefinition->east));

         if (NULL == connected) {
            g_outputError("error: room '%s' connects to room '%s', but "
               "room '%s' doesn't exist!\n", dstrview(room->name),
               dstrview(roomDefinition->east), dstrview(roomDefinition->east));
            exit(EXIT_FAILURE);
         }

         room->east = connected;
      }

      if (NULL != roomDefinition->west) {

         connected = g_hash_table_lookup(g_rooms, dstrview(roomDefinition->west));

         if (NULL == connected) {
            g_outputError("error: room '%s' connects to room '%s', but "
               "room '%s' doesn't exist!\n", dstrview(room->name),
               dstrview(roomDefinition->west), dstrview(roomDefinition->west));
            exit(EXIT_FAILURE);
         }

         room->west = connected;
      }

      if (NULL != roomDefinition->in) {

         connected = g_hash_table_lookup(g_rooms, dstrview(roomDefinition->in));

         if (NULL == connected) {
            g_outputError("error: room '%s' connects to room '%s', but "
               "room '%s' doesn't exist!\n", dstrview(room->name),
               dstrview(roomDefinition->in), dstrview(roomDefinition->in));
            exit(EXIT_FAILURE);
         }

         room->in = connected;
      }

      if (NULL != roomDefinition->out) {

         connected = g_hash_table_lookup(g_rooms, dstrview(roomDefinition->out));

         if (NULL == connected) {
            g_outputError("error: room '%s' connects to room '%s', but "
               "room '%s' doesn't exist!\n", dstrview(room->name),
               dstrview(roomDefinition->out), dstrview(roomDefinition->out));
            exit(EXIT_FAILURE);
         }

         room->out = connected;
      }

      if (NULL != roomDefinition->up) {

         connected = g_hash_table_lookup(g_rooms, dstrview(roomDefinition->up));

         if (NULL == connected) {
            g_outputError("error: room '%s' connects to room '%s', but "
               "room '%s' doesn't exist!\n", dstrview(room->name),
               dstrview(roomDefinition->up), dstrview(roomDefinition->up));
            exit(EXIT_FAILURE);
         }

         room->up = connected;
      }

      if (NULL != roomDefinition->down) {

         connected = g_hash_table_lookup(g_rooms, dstrview(roomDefinition->down));

         if (NULL == connected) {
            g_outputError("error: room '%s' connects to room '%s', but "
               "room '%s' doesn't exist!\n", dstrview(room->name),
               dstrview(roomDefinition->down), dstrview(roomDefinition->down));
            exit(EXIT_FAILURE);
         }

         room->down = connected;
      }

      currentRoomName = currentRoomName->next;
   }

   g_list_free(roomNames);
   return;
}

/******************************************************************************/

static GList *addObject(GList *list, GHashTable *byName, dstring_t name) {

   int i;
   GList *synonymList;
   Object *object = g_hash_table_lookup(g_objects, dstrview(name));

   /* add object to the linked list for iteration */
   list = g_list_append(list, object);

   /* make sure object can also be referenced by name */
   synonymList = g_hash_table_lookup(byName, (char *)dstrview(name));
   synonymList = g_list_append(synonymList, object);
   g_hash_table_insert(byName, (char *)dstrview(name), synonymList);

   /* also reference objects by synonyms */
   for (i = 0; i < object->synonyms->len; i++) {
      #define OBJ_SYNONYM g_array_index(object->synonyms, dstring_t, i)
      synonymList = g_hash_table_lookup(byName, (char *)dstrview(OBJ_SYNONYM));
      synonymList = g_list_append(synonymList, object);
      g_hash_table_insert(byName, (char *)dstrview(OBJ_SYNONYM), synonymList);
      #undef OBJ_SYNONYM
   }

   return list;
}

/******************************************************************************/

static void printParsedObject(ObjectParsed *object) {

   printf("Name: %s\n", dstrview(object->name));
   printf("Description: %s\n", dstrview(object->description));
}

/******************************************************************************/

static void printParsedRoom(RoomParsed *room) {

   int i = 0;

   printf("Name: %s\n", dstrview(room->name));
   printf("Description: %s\n", dstrview(room->description));

   if (NULL != room->north) {
      printf("North: %s\n", dstrview(room->north));
   }

   if (NULL != room->south) {
      printf("South: %s\n", dstrview(room->south));
   }

   if (NULL != room->east) {
      printf("East: %s\n", dstrview(room->east));
   }

   if (NULL != room->west) {
      printf("West: %s\n", dstrview(room->west));
   }

   if (NULL != room->objects) {
      printf("Objects: ");
      for (i = 0; i < room->objects->len; i++) {
         printf("%s, ", dstrview(g_array_index(room->objects, dstring_t, i)));
      }

      printf("\n");
   }

   printf("\n");
}

