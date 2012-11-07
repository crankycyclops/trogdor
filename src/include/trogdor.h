
#ifndef TROGDOR_H
#define TROGDOR_H


#include <glib.h>
#include <dstring.h>

/* out of memory errors happen often enough to write a macro */
#define PRINT_OUT_OF_MEMORY_ERROR {fprintf(stderr, "out of memory"); exit(EXIT_FAILURE);}


/* Synonyms will be stored as keys in a hash table */
typedef struct object {
   dstring_t name;         /* name of the object */
   dstring_t description;  /* the user reads this when seen the first time */
   int seen;               /* whether or not the object has been seen */
} Object;

/* Represents a single room. Each direction is a pointer to whatever room is
   located in that direction.  If you can't go in that direction, it should be
   set to null.
*/
typedef struct room {
   char *description;
   struct room *north;
   struct room *south;
   struct room *east;
   struct room *west;

   /* A lookup table that maps object names and synonyms to actual objects.
      There may be ambiguities where more than one object has the same synonym.
      We therefore set as the value for each key a GArray.  If there is only one
      object associated with a given word (name or synonym), we know which
      object the name resolves to.  If we get more than one in the array, we
      have an ambiguity that must be resolved through further clarification from
      the user. */
   GHashTable *objectByName;

   /* We also need an iterable array of all objects in a room so that we can
      quickly operate on all objects when we enter a room. */
   GArray *objectList;
} Room;


#endif

