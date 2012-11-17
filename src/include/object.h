#ifndef OBJECT_H
#define OBJECT_H


#include <dstring.h>


/* represents the state of a game object */
typedef struct objectState {
   int seenByPlayer;    /* whether object has been seen by the player */
   int takenByPlayer;   /* whether object has been taken by the player */
   int droppedByPlayer; /* whether object has been dropped by the player */
} ObjectState;

/* Synonyms will be stored as keys in a hash table */
typedef struct object {

   dstring_t name;         /* name of the object */
   dstring_t description;  /* the user reads this when seen the first time */
   ObjectState state;

   /* an array of synonyms (dstring_t's) */
   GArray *synonyms;

} Object;


#ifndef OBJECT_C

/* prints description of an object */
extern void displayObject(Object *object);

/* processes the posession of an object from the current room */
extern void takeObject(Object *object);

/* drops the specified object into the current room */
extern void dropObject(Object *object);

/* disambiguates in the case where a name refers to more than one object */
extern Object *clarifyObject(GList *objects, int objectCount);

#endif


#endif

