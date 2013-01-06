
#define ATTRIBUTES_C
#include <stdio.h>
#include "include/attributes.h"

/* calculates normalized strength relative to initial values */
double calcStrengthFactor(Attributes attributes);

/* calculates normalized dexterity relative to initial values */
double calcDexterityFactor(Attributes attributes);

/* calculates normalized intelligence relative to initial values */
double calcIntelligenceFactor(Attributes attributes);

/******************************************************************************/

double calcStrengthFactor(Attributes attributes) {

   return (double)attributes.strength / (double)attributes.initialTotal;
}

/******************************************************************************/

double calcDexterityFactor(Attributes attributes) {

   return (double)attributes.dexterity / (double)attributes.initialTotal;
}

/******************************************************************************/

double calcIntelligenceFactor(Attributes attributes) {

   return (double)attributes.intelligence / (double)attributes.initialTotal;
}

