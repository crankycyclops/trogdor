#ifndef OBJECT_H
#define OBJECT_H


/* prints description of an object */
extern void displayObject(Object *object);

/* processes the posession of an object from the current room */
extern void takeObject(Object *object);

/* drops the specified object into the current room */
extern void dropObject(Object *object);

/* disambiguates in the case where a name refers to more than one object */
extern Object *clarifyObject(GList *objects, int objectCount);


#endif

