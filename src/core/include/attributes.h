#ifndef ATTRIBUTES_H
#define ATTRIBUTES_H


typedef struct {

   int strength;
   int dexterity;
   int intelligence;

   /* total number of attributes when first initialized */
   int initialTotal;

} Attributes;

/* if no configured values, these are the defaults (an even spread) */
#define DEFAULT_PLAYER_STRENGTH        10
#define DEFAULT_PLAYER_DEXTERITY       10
#define DEFAULT_PLAYER_INTELLIGENCE    10
#define DEFAULT_CREATURE_STRENGTH      10
#define DEFAULT_CREATURE_DEXTERITY     10
#define DEFAULT_CREATURE_INTELLIGENCE  10

#ifndef ATTRIBUTES_C

/* calculates normalized strength relative to initial values */
extern double calcStrengthFactor(Attributes attributes);

/* calculates normalized dexterity relative to initial values */
extern double calcDexterityFactor(Attributes attributes);

/* calculates normalized intelligence relative to initial values */
extern double calcIntelligenceFactor(Attributes attributes);

#endif


#endif

