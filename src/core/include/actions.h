
#ifndef ACTIONS_H
#define ACTIONS_H


#include "command.h"

/* psuedo action that frees allocated memory and quits the game */
extern int actionQuit(Player *player, Command command);

/* moves the user in the specified direction */
extern int actionMove(Player *player, Command command);

/* look at objects or describe the room again */
extern int actionLook(Player *player, Command command);

/* list the items in the user's inventory */
extern int actionList(Player *player, Command command);

/* allows the user to pick up an object (requires direct object) */
extern int actionPickupObject(Player *player, Command command);

/* allows the user to drop an object */
extern int actionDropObject(Player *player, Command command);

/* allows the user to jump */
extern int actionJump(Player *player, Command command);


#endif

