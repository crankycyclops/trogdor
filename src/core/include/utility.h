#ifndef UTILITY_H
#define UTILITY_H


/* returns TRUE if a string is an integer and FALSE if it's not */
extern int isInt(char *str);

/* returns TRUE if a string is a float and FALSE if it's not */
extern int isDouble(char *str);

/* allocate a new dstring_t object */
extern dstring_t createDstring();

/* macro to clip a value within a given range */
#define CLIP(V, MIN, MAX)  (((V) > (MAX)) ? (MAX) : (((V) < (MIN)) ? (MIN) : (V)))


#endif

