
#ifndef ACTIONS_H
#define ACTIONS_H


/* moves the user in the specified direction */
extern int move(Command command);

/* psuedo action that frees allocated memory and quits the game */
extern int quitGame(Command command);


#endif

