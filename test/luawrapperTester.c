/* Reference: http://gamedevgeek.com/tutorials/calling-lua-functions/ */
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <luaunpanic.h>

lua_State* L;

#define PRINT_ERROR_STRING(L, f) do {					\
    char *errorstring = (char *) lua_tostring(L, -1);;			\
    if (errorstring == NULL) {                                          \
      fprintf(stderr, "%s error\n", #f);				\
    } else {								\
      fprintf(stderr, "%s error: %s\n", #f, errorstring);               \
    }									\
  } while (0)

short luaadd(int *rcp, int x, int y)
{
  int rc;
  int sum;
  lua_Integer luaInteger;

  /* the function name */
  if (luaunpanic_getglobal(&rc, L, "add")) {
    PRINT_ERROR_STRING(L, luaunpanic_getglobal);
    return 1;
  }

  if (rc != LUA_TFUNCTION) {
    fprintf(stderr, "\"add\" is not a function, type is %d != LUA_TFUNCTION\n", rc);
    return 1;
  }

  /* the first argument */
  if (luaunpanic_pushnumber(L, (lua_Number) x)) {
    PRINT_ERROR_STRING(L, luaunpanic_pushnumber);
    return 1;
  }

  /* the second argument */
  if (luaunpanic_pushnumber(L, (lua_Number) y)) {
    PRINT_ERROR_STRING(L, luaunpanic_pushnumber);
    return 1;
  }

  /* call the function with 2 arguments, return 1 result */
  if (luaunpanic_call(L, 2, 1)) {
    PRINT_ERROR_STRING(L, luaunpanic_call);
    return 1;
  }

  /* get the result */
  if (luaunpanic_tointeger(&luaInteger, L, -1)) {
    PRINT_ERROR_STRING(L, luaunpanic_tointeger);
    return 1;
  }
  sum = (int)luaInteger;

  if (luaunpanic_pop(L, 1)) {
    PRINT_ERROR_STRING(L, luaunpanic_pop);
    return 1;
  }

  *rcp = sum;
  return 0;
}

short luadoerror()
{
  int rc;

  /* the function name */
  if (luaunpanic_getglobal(&rc, L, "doerror")) {
    PRINT_ERROR_STRING(L, luaunpanic_getglobal);
    return 1;
  }

  if (rc != LUA_TFUNCTION) {
    fprintf(stderr, "\"doerror\" is not a function, type is %d != LUA_TFUNCTION\n", rc);
    return 1;
  }

  /* call the function with 0 arguments, ignore result */
  if (luaunpanic_call(L, 0, 0)) {
    PRINT_ERROR_STRING(L, luaunpanic_call);
    /* We expect to fail! */
    return 0;
  }

  /* we do not expect to succeed */
  return 1;
}

int main(int argc, char *argv[])
{
  int sum;
  int rc;

  if (argc != 2) {
    fprintf(stderr, "Usage: %s path/to/add.lua\n", argv[0]);
    exit(1);
  }

  /* initialize Lua */
  if (luaunpanicL_newstate(&L)) {
    fprintf(stderr, "luaunpanicL_newstate failure, %s\n", strerror(errno));
    exit(1);
  }

  /* load Lua base libraries */
  if (luaunpanicL_openlibs(L)) {
    PRINT_ERROR_STRING(L, luaunpanicL_openlibs);
    exit(1);
  }

  /* load the script */
  if (luaunpanicL_dofile(&rc, L, argv[1]) || rc) {
    PRINT_ERROR_STRING(L, luaunpanicL_dofile);
    exit(1);
  }

  /* call the add function */
  if (luaadd(&sum, 10, 15)) {
    fprintf(stderr, "luaadd error\n");
    exit(1);
  }
  printf( "The sum is %d\n", sum );

  /* call the doerror function */
  if (luadoerror() != 0) {
    fprintf(stderr, "luadoerror error\n");
    exit(1);
  }

  /* Clear the stack */
  if (luaunpanic_settop(L, 0)) {
    PRINT_ERROR_STRING(L, luaunpanic_settop);
    exit(1);
  }

  /* call an explicit error function to verify they do not abort */
  if (! luaunpanic_error(&rc, L)) {
    fprintf(stderr, "luaunpanic_error returns success!?\n");
    exit(1);
  } else {
    fprintf(stdout, "luaunpanic_error returns failure, error string follows: \n");
    PRINT_ERROR_STRING(L, luaunpanic_error);
  }

  /* Clear the stack */
  if (luaunpanic_settop(L, 0)) {
    PRINT_ERROR_STRING(L, luaunpanic_settop);
    exit(1);
  }

  /* call an explicit error function to verify they do not abort */
  if (! luaunpanicL_argerror(&rc, L, 1, "extramsg")) {
    fprintf(stderr, "luaunpanicL_argerror returns success!?\n");
    exit(1);
  } else {
    fprintf(stdout, "luaunpanicL_argerror returns failure, error string follows: \n");
    PRINT_ERROR_STRING(L, luaunpanic_error);
  }

  /* Clear the stack */
  if (luaunpanic_settop(L, 0)) {
    PRINT_ERROR_STRING(L, luaunpanic_settop);
    exit(1);
  }

  /* call an explicit error function to verify they do not abort */
  if (! luaunpanicL_error (&rc, L, "%s %s %s", "My", "private", "error string")) {
    fprintf(stderr, "luaunpanicL_error returns success!?\n");
    exit(1);
  } else {
    fprintf(stdout, "luaunpanicL_error returns failure, error string follows: \n");
    PRINT_ERROR_STRING(L, luaunpanic_error);
  }

  /* cleanup Lua */
  if (luaunpanic_close(L)) {
    fprintf(stderr, "luaunpanic_close error\n");
    exit(1);
  }

  exit(0);
}
