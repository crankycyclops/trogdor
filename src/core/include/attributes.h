#ifndef ATTRIBUTES_H
#define ATTRIBUTES_H


typedef struct {
   int strength;
   int dexterity;
   int intelligence;
} Attributes;

/* if no configured values, these are the defaults (an even spread) */
#define DEFAULT_PLAYER_STRENGTH        10
#define DEFAULT_PLAYER_DEXTERITY       10
#define DEFAULT_PLAYER_INTELLIGENCE    10
#define DEFAULT_CREATURE_STRENGTH      10
#define DEFAULT_CREATURE_DEXTERITY     10
#define DEFAULT_CREATURE_INTELLIGENCE  10

/* macros to calculate commonly needed data about attributes */
#define ATTRIBUTES_TOTAL(X) (X.strength + X.dexterity + X.intelligence)
#define STRENGTH_FACTOR(X) (X.strength / (double)ATTRIBUTES_TOTAL(X))
#define DEXTERITY_FACTOR(X) (X.dexterity / (double)ATTRIBUTES_TOTAL(X))
#define INTELLIGENCE_FACTOR(X) (X.intelligence / (double)ATTRIBUTES_TOTAL(X))


#endif

