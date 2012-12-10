#ifndef CONSTRUCT_H
#define CONSTRUCT_H


#include <dstring.h>
#include <glib.h>

typedef struct roomParsed {

   dstring_t name;
   dstring_t title;
   dstring_t description;
   dstring_t north;
   dstring_t south;
   dstring_t east;
   dstring_t west;

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

   /* an array of synonyms (dstring_t's) */
   GArray *synonyms;

   /* array of filenames containing Lua scripts */
   GArray *scripts;

} ObjectParsed;

typedef struct creatureParsed {

   dstring_t name;
   dstring_t title;
   dstring_t description;
   dstring_t deadDesc;

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

