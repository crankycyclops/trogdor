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

/* initialize global event handlers */
static void initEvents();

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
static lua_State *initLuaState(GList *scriptList);

/* connect rooms so that users can navigate north, south, etc. */
static void connectRooms();

/* adds an object to a linked list and a hash table (by name and synonym) */
static GList *addObject(GList *list, GHashTable *byName, dstring_t name);

/* adds a creature to a linked list and a hash table (by name and synonym) */
static GList *addCreature(GList *list, GHashTable *byName, dstring_t name);

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

/* list of parsed global script names */
GList *globalScriptsParsed = NULL;

/* list of parsed global event handlers */
GList *globalEventHandlersParsed = NULL;

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

   initEvents();
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

static void initEvents() {

   GList *nextEventHandler = globalEventHandlersParsed;

   globalL = initLuaState(globalScriptsParsed);

   while (nextEventHandler != NULL) {
      EventHandlerParsed *handler = (EventHandlerParsed *)nextEventHandler->data;
      addGlobalEventHandler(dstrview(handler->event), dstrview(handler->function));
      nextEventHandler = g_list_next(nextEventHandler);
   }
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
   GList *nextEventHandler;
   Creature *creature = createCreature(FALSE);
   GArray *objectNames = creatureParsed->objects;

   creature->name = creatureParsed->name;
   creature->title = creatureParsed->title;
   creature->description = creatureParsed->description;
   creature->messages = creatureParsed->messages;
   creature->attackable = creatureParsed->attackable;
   creature->woundRate = creatureParsed->woundRate;
   creature->counterattack = creatureParsed->counterattack;

   /* creature's attributes */
   creature->attributes.strength = creatureParsed->attributes.strength;
   creature->attributes.dexterity = creatureParsed->attributes.dexterity;
   creature->attributes.intelligence = creatureParsed->attributes.intelligence;
   creature->attributes.initialTotal = creature->attributes.strength +
      creature->attributes.dexterity + creature->attributes.intelligence;

   /* health settings */
   creature->maxHealth = creatureParsed->maxHealth;
   creature->state.health = creatureParsed->health;
   creature->state.alive = creatureParsed->alive;

   if (NULL != creatureParsed->allegiance) {

      if (0 == strcmp(dstrview(creatureParsed->allegiance), "neutral")) {
         creature->allegiance = CREATURE_ALLEGIANCE_NEUTRAL;
      }
      
      else if (0 == strcmp(dstrview(creatureParsed->allegiance), "friend")) {
         creature->allegiance = CREATURE_ALLEGIANCE_FRIEND;
      }
      
      else if (0 == strcmp(dstrview(creatureParsed->allegiance), "enemy")) {
         creature->allegiance = CREATURE_ALLEGIANCE_ENEMY;
      }
      
      else {
         g_outputError("%s is not a valid allegiance for creature '%s'!\n",
            dstrview(creatureParsed->allegiance),
            dstrview(creatureParsed->name));
         exit(EXIT_FAILURE);
      }
   }

   /* initialize lua state for creature */
   creature->L = initLuaState(creatureParsed->scripts);

   /* initialize entity-specific event handlers */
   nextEventHandler = creatureParsed->eventHandlers;
   while (NULL != nextEventHandler) {
      EventHandlerParsed *handler = (EventHandlerParsed *)nextEventHandler->data;
      addEntityEventHandler(dstrview(handler->event), creature, entity_creature,
         dstrview(handler->function), creature->L);
      nextEventHandler = g_list_next(nextEventHandler);
   }

   /* add objects to the creature's inventory */
   for (i = 0; i < objectNames->len; i++) {

      dstring_t *objectName = g_array_index(objectNames, dstring_t, i);
      Object *object = g_hash_table_lookup(g_objects, (char *)dstrview(objectName));

      creature->inventory.list = addObject(creature->inventory.list,
         creature->inventory.byName, objectName);
      object->state.owner.entity = creature;
      object->state.owner.type = entity_creature;
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
   GList *nextEventHandler;

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

   /* add creatures to the room */
   for (i = 0; i < creatureNames->len; i++) {
      room->creatureList = addCreature(room->creatureList, room->creatureByName,
         g_array_index(creatureNames, dstring_t, i));
   }

   /* add objects to the room */
   for (i = 0; i < objectNames->len; i++) {

      dstring_t *objectName = g_array_index(objectNames, dstring_t, i);
      Object *object = g_hash_table_lookup(g_objects, (char *)dstrview(objectName));

      room->objectList = addObject(room->objectList, room->objectByName, objectName);
      object->state.owner.entity = room;
      object->state.owner.type = entity_room;
   }

   /* initialize lua state for room */
   room->L = initLuaState(roomParsed->scripts);

   /* initialize entity-specific event handlers */
   nextEventHandler = roomParsed->eventHandlers;
   while (NULL != nextEventHandler) {
      EventHandlerParsed *handler = (EventHandlerParsed *)nextEventHandler->data;
      addEntityEventHandler(dstrview(handler->event), room, entity_room,
         dstrview(handler->function), room->L);
      nextEventHandler = g_list_next(nextEventHandler);
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

   GList *nextEventHandler;
   Object *object = createObject(FALSE);

   object->name = objectParsed->name;
   object->description = objectParsed->description;
   object->weight = objectParsed->weight;
   object->takeable = objectParsed->takeable;
   object->droppable = objectParsed->droppable;
   object->messages = objectParsed->messages;

   object->weapon = objectParsed->weapon;
   object->damage = objectParsed->damage;

   object->synonyms = objectParsed->synonyms;

   /* initialize lua state for object */
   object->L = initLuaState(objectParsed->scripts);

   /* initialize entity-specific event handlers */
   nextEventHandler = objectParsed->eventHandlers;
   while (NULL != nextEventHandler) {
      EventHandlerParsed *handler = (EventHandlerParsed *)nextEventHandler->data;
      addEntityEventHandler(dstrview(handler->event), object, entity_object,
         dstrview(handler->function), object->L);
      nextEventHandler = g_list_next(nextEventHandler);
   }

   return object;
}

/******************************************************************************/

static lua_State *initLuaState(GList *scriptList) {

   GList *nextScript = scriptList;
   lua_State *L = NULL;

   while (NULL != nextScript) {

      /* only initialize lua state if we have something to execute */
      if (NULL == L) {
         L = luaL_newstate();
         initLua(L);
      }

      if (loadScript(L, (char *)dstrview((dstring_t)nextScript->data))) {
         primeLua(L);
      }

      nextScript = g_list_next(nextScript);
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
   ObjectParsed *objDefinition = g_hash_table_lookup(objectParsedTable,
      dstrview(name));
   Object *object = g_hash_table_lookup(g_objects, dstrview(name));

   /* make sure object isn't going into more than one location */
   objDefinition->used++;
   if (objDefinition->used > 1) {
      fprintf(stderr, "error: object '%s' exists in more than one location!\n",
         dstrview(name));
      exit(EXIT_FAILURE);
   }

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

static GList *addCreature(GList *list, GHashTable *byName, dstring_t name) {

   int i;
   GList *synonymList;
   CreatureParsed *creatureDef = g_hash_table_lookup(creatureParsedTable,
      dstrview(name));
   Creature *creature = g_hash_table_lookup(g_creatures, dstrview(name));

   /* make sure object isn't going into more than one location */
   creatureDef->used++;
   if (creatureDef->used > 1) {
      fprintf(stderr, "error: creature '%s' exists in more than one location!\n",
         dstrview(name));
      exit(EXIT_FAILURE);
   }

   /* add creature to the linked list for iteration */
   list = g_list_append(list, creature);

   /* make sure object can also be referenced by name */
   synonymList = g_hash_table_lookup(byName, (char *)dstrview(name));
   synonymList = g_list_append(synonymList, creature);
   g_hash_table_insert(byName, (char *)dstrview(name), synonymList);

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

