
#ifndef ACTIONS_H
#define ACTIONS_H


/* moves the user in the specified direction */
extern int move(Command command);

/* look at objects or describe the room again */
extern int look(Command command);

/* allows the user to pick up an object (requires direct object) */
extern int pickupObject(Command command);

/* psuedo action that frees allocated memory and quits the game */
extern int quitGame(Command command);


#endif

