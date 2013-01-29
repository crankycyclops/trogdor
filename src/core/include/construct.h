#ifndef CONSTRUCT_H
#define CONSTRUCT_H


#include <dstring.h>
#include <glib.h>

#include "attributes.h"
#include "messages.h"


typedef struct {
   dstring_t event;
   dstring_t function;
} EventHandlerParsed;

typedef struct {

   dstring_t name;
   dstring_t title;
   dstring_t description;

   dstring_t north;
   dstring_t south;
   dstring_t east;
   dstring_t west;

   dstring_t in;
   dstring_t out;
   dstring_t up;
   dstring_t down;

   /* hash table of custom messages */
   Messages messages;

   /* an array of object identifiers (dstring_t's) */
   GArray *objects;

   /* names of all creatures in the room (dstring_t's) */
   GArray *creatures;

   /* list of parsed script filenames */
   GList *scripts;

   /* list of parsed event handlers */
   GList *eventHandlers;

} RoomParsed;

typedef struct {

   dstring_t name;
   dstring_t description;

   int weight;

   int takeable;
   int droppable;

   /* how many times the object was placed in a room or inventory */
   int used;

   /* combat-related properties */
   int weapon;
   int damage;

   /* hash table of custom messages */
   Messages messages;

   /* an array of synonyms (dstring_t's) */
   GArray *synonyms;

   /* list of parsed script filenames */
   GList *scripts;

   /* list of parsed event handlers */
   GList *eventHandlers;

} ObjectParsed;

typedef struct {

   dstring_t name;
   dstring_t title;
   dstring_t description;
   dstring_t allegiance;

   Attributes attributes;

   /* values related to creature's health */
   int health;
   int maxHealth;
   int alive;

   /* whether or not we can attack the creature */
   int attackable;
   int counterattack;
   double woundRate;

   /* how many times the creature was placed in a room */
   int used;

   /* hash table of custom messages */
   Messages messages;

   /* array of object identifiers (dstring_t's) */
   GArray *objects;

   /* list of filenames containing Lua scripts */
   GList *scripts;

   /* list of parsed event handlers */
   GList *eventHandlers;

} CreatureParsed;


/* a lookup table for game objects being parsed */
extern GHashTable *objectParsedTable;

/* a lookup table for rooms being parsed */
extern GHashTable *roomParsedTable;

/* a lookup table for creatures being parsed */
extern GHashTable *creatureParsedTable;

/* list of parsed global script names */
extern GList *globalScriptsParsed;

/* list of parsed global event handlers */
extern GList *globalEventHandlersParsed;


/* entry point for parsing the game file */
extern int parseGame(const char *filename);


#endif

