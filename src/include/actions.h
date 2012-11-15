
#ifndef ACTIONS_H
#define ACTIONS_H


#include "command.h"

/* psuedo action that frees allocated memory and quits the game */
extern int actionQuit(Command command);

/* moves the user in the specified direction */
extern int actionMove(Command command);

/* look at objects or describe the room again */
extern int actionLook(Command command);

/* allows the user to pick up an object (requires direct object) */
extern int actionPickupObject(Command command);

/* allows the user to drop an object */
int actionDropObject(Command command);


#endif

