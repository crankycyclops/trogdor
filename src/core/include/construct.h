#ifndef CONSTRUCT_H
#define CONSTRUCT_H


#include <dstring.h>
#include <glib.h>

#include "messages.h"


typedef struct roomParsed {

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
   
} RoomParsed;

typedef struct objectParsed {

   dstring_t name;
   dstring_t description;
   dstring_t weight;
   dstring_t takeable;
   dstring_t droppable;

   /* how many times the object was placed in a room or inventory */
   int used;

   /* hash table of custom messages */
   Messages messages;

   /* an array of synonyms (dstring_t's) */
   GArray *synonyms;

   /* array of filenames containing Lua scripts */
   GArray *scripts;

} ObjectParsed;

typedef struct creatureParsed {

   dstring_t name;
   dstring_t title;
   dstring_t description;
   dstring_t allegiance;

   /* how many times the creature was placed in a room */
   int used;

   /* hash table of custom messages */
   Messages messages;

   /* an array of object identifiers (dstring_t's) */
   GArray *objects;

   /* array of filenames containing Lua scripts */
   GArray *scripts;

} CreatureParsed;


/* a lookup table for game objects being parsed */
extern GHashTable *objectParsedTable;

/* a lookup table for rooms being parsed */
extern GHashTable *roomParsedTable;

/* a lookup table for creatures being parsed */
extern GHashTable *creatureParsedTable;


/* entry point for parsing the game file */
extern int parseGame(const char *filename);


#endif

