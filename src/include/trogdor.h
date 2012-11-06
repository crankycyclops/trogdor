
#ifndef TROGDOR_H
#define TROGDOR_H


#include <dstring.h>

/* out of memory errors happen often enough to write a macro */
#define PRINT_OUT_OF_MEMORY_ERROR {fprintf(stderr, "out of memory"); exit(EXIT_FAILURE);}

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
} Room;

/* Synonyms will be keys in the objects hash table */
typedef struct object {
   dstring_t name;         /* name of the object */
   dstring_t description;  /* the user reads this when seen the first time */
   int seen;               /* whether or not the object has been seen */
} Object;


#endif

