
#ifndef ACTION_H
#define ACTION_H


#include "trogdor.h"
#include "state.h"
#include "command.h"

/* executes an action corresponding to the command's verb */
extern int callAction(Player *player, Command command);


#endif

