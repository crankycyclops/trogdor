#ifndef ATTRIBUTES_H
#define ATTRIBUTES_H


typedef struct {
   int strength;
   int dexterity;
   int intelligence;
} Attributes;

/* macros to calculate commonly needed data about attributes */
#define ATTRIBUTES_TOTAL(X) (X.strength + X.dexterity + X.intelligence)
#define STRENGTH_FACTOR(X) (X.strength / ATTRIBUTES_TOTAL(X))
#define DEXTERITY_FACTOR(X) (X.dexterity / ATTRIBUTES_TOTAL(X))
#define INTELLIGENCE_FACTOR(X) (X.intelligence / ATTRIBUTES_TOTAL(X))


#endif

