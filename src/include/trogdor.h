
#ifndef TROGDOR_H
#define TROGDOR_H


/* directions we can move in during the game */
#define NORTH 1
#define SOUTH 2
#define EAST  3
#define WEST  4

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


#endif

