#ifndef SCRIPT_H
#define SCRIPT_H


#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

/* initializes a Lua state with our C-to-Lua API */
extern void initLua(lua_State *L);

/* loads a Lua script from a file */
extern void loadScript(lua_State *L, char *filename);

/* primes Lua state so that all variables and functions get loaded */
extern void primeLua(lua_State *L);

/* This gets set to 1 if we had trouble parsing a script.  Using this will allow
   us to delay exiting due to the error until we've parsed and reported errors
   for all possibly bad scripts. */
extern int g_scriptError;


#endif

