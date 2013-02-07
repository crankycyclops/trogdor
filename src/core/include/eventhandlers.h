#ifndef EVENTHANDLERS_H
#define EVENTHANDLERS_H


#include "trogdor.h"
#include "room.h"
#include "object.h"
#include "creature.h"
#include "player.h"
#include "timer.h"
#include "event.h"
#include "data.h"

#ifndef EVENTHANDLERS_C

/* Responds to afterSetLocation event and automatically initiates an attack
   against the player by any creatures that have auto-attack turned on. */
extern int eventHandlerAutoAttack(int nargs, va_list args);

#endif


#endif

