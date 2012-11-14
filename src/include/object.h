#ifndef OBJECT_H
#define OBJECT_H


/* processes the posession of an object from the current room */
extern void takeObject(Object *object);

/* disambiguates in the case where a name refers to more than one object */
extern Object *clarifyObject(GList *objects, int objectCount);


#endif

