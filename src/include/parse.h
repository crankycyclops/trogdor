#ifndef PARSE_H
#define PARSE_H


#include <dstring.h>
#include <glib.h>

typedef struct roomParsed {

   dstring_t name;
   dstring_t description;
   dstring_t north;
   dstring_t south;
   dstring_t east;
   dstring_t west;

   /* an array of object identifiers */
   dstring_t *objects;
   
} RoomParsed;

typedef struct objectParsed {

   dstring_t name;
   dstring_t description;

   /* an array of synonyms */
   dstring_t *synonyms;
} ObjectParsed;


/* rooms that have been parsed from the XML game file */
extern RoomParsed **parsedRooms;

/* game objects that have been parsed from the XML game file */
extern ObjectParsed **parsedObjects;

/* a lookup table for game objects being parsed */
extern GHashTable *objectParsedTable;


/* initializes the game file parser */
extern void initParser();

/* frees memory associated with the game file parser */
extern void destroyParser();


#endif

