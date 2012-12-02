#ifndef VOCABULARY_H
#define VOCABULARY_H


#include "command.h"

typedef struct verb {
   char *word;                     /* verb that maps to a specific action */
   int (*action)(Player *, Command); /* pointer to the action we call for the verb */
} Verb;


#ifndef VOCABULARY_C

/* list of English filler words that we should ignore */
extern const char *fillerWords[];

/* list of one-word English prepositions recognized by Trogdor */
extern const char *prepositions[];

/* return our list of verb => action mappings */
extern Verb *getVerbs();

#endif


#endif

