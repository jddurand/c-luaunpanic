CMAKE_MINIMUM_REQUIRED (VERSION 3.26.0 FATAL_ERROR)
PROJECT(luaunpanic VERSION 5.3.4 LANGUAGES C)
IF (NOT MYPACKAGEBOOTSTRAP_DONE)
  INCLUDE ("3rdparty/github/cmake-utils/cmake/MyPackageBootstrap.cmake")
ENDIF ()
MYPACKAGESTART ()

OPTION (LUA_32BITS "Use 32-bit integers and 32-bit floats" OFF)
OPTION (LUA_USE_C89 "Avoid the use of few C99 feature" OFF)

###########
# Library #
###########
MYPACKAGELIBRARY(
  ${CMAKE_CURRENT_SOURCE_DIR}/include/luaunpanic/internal/luaconf.h.in
  ${INCLUDE_OUTPUT_PATH}/luaunpanic/lua/luaconf.h
  src/luaunpanic_amalgamation.c)

#
# OS-specifics
#
IF (WIN32 AND NOT CYGWIN)
  TARGET_COMPILE_DEFINITIONS(${PROJECT_NAME}_iface INTERFACE -DLUA_DL_DLL)
ELSE ()
  TARGET_COMPILE_DEFINITIONS(${PROJECT_NAME}_iface INTERFACE -DLUA_USE_DLOPEN -DLUA_USE_POSIX)
ENDIF ()

#
# We moved lua headers to include/luaunpanic/lua
# We generate luaconf.h in ${INCLUDE_OUTPUT_PATH}/luaunpanic/lua
#
TARGET_INCLUDE_DIRECTORIES(${PROJECT_NAME}_iface INTERFACE $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include/luaunpanic/lua> $<INSTALL_INTERFACE:include/luaunpanic/lua>)
#
# Add lua/luaconf.h to interface
#
TARGET_SOURCES(${PROJECT_NAME}_iface INTERFACE FILE_SET public_headers BASE_DIRS ${PUBLIC_HEADERS_BASE_DIRS} TYPE HEADERS FILES ${INCLUDE_OUTPUT_PATH}/luaunpanic/lua/luaconf.h)
TARGET_INCLUDE_DIRECTORIES(${PROJECT_NAME}_iface INTERFACE $<BUILD_INTERFACE:${INCLUDE_OUTPUT_PATH}/luaunpanic/lua>)
#
# dl and m dependency
#
IF (CMAKE_DL_LIBS OR CMAKE_MATH_LIBS)
  FOREACH (_target ${PROJECT_NAME}_shared ${PROJECT_NAME}_static)
    IF (CMAKE_DL_LIBS)
      TARGET_LINK_LIBRARIES(${_target} PUBLIC ${CMAKE_DL_LIBS})
    ENDIF ()
    IF (CMAKE_MATH_LIBS)
      TARGET_LINK_LIBRARIES(${_target} PUBLIC ${CMAKE_MATH_LIBS})
    ENDIF ()
  ENDFOREACH ()
ENDIF ()
#
# Those definitions have to propagate to those using the library
#
SET (LUA_PUBLIC_CFLAGS)
IF (LUA_32BITS)
  LIST (APPEND LUA_PUBLIC_CFLAGS "-DLUA_32BITS")
ENDIF ()
IF (LUA_USE_C89)
  LIST (APPEND LUA_PUBLIC_CFLAGS "-DLUA_USE_C89")
ENDIF ()

LIST(LENGTH LUA_PUBLIC_CFLAGS LUA_PUBLIC_CFLAGS_LENGTH)
IF (LUA_PUBLIC_CFLAGS_LENGTH GREATER 0)
  FOREACH (_target luaunpanic luaunpanic_static luaunpanic_objs luaunpanic_static_objs)
    TARGET_COMPILE_DEFINITIONS(${_target} PUBLIC ${LUA_PUBLIC_CFLAGS})
  ENDFOREACH ()
ENDIF ()

#
# Those definitions are private
#
SET (LUA_PRIVATE_C_FLAGS)
IF (CMAKE_C_COMPILER_ID STREQUAL SunPro)
  SET (LUA_PRIVATE_C_FLAGS "-xc99")
ENDIF ()

IF (NOT "x${LUA_PRIVATE_C_FLAGS}" STREQUAL x)
  #
  # Fortunately this will remain local to this CMakeLists.txt -;
  #
  SET (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${LUA_PRIVATE_C_FLAGS}")
ENDIF ()

###############
# Executables #
###############
MYPACKAGETESTEXECUTABLE(luaunpanicwrapperTester test/luawrapperTester.c)
MYPACKAGETESTEXECUTABLE(luaunpanicwrapperTester2 test/luawrapperTester2.c)

#########
# Tests #
#########
MYPACKAGECHECK(luaunpanicwrapperTester ${CMAKE_CURRENT_SOURCE_DIR}/test/luawrapperTester.lua)
MYPACKAGECHECK(luaunpanicwrapperTester2 ${CMAKE_CURRENT_SOURCE_DIR}/test/luawrapperTester2.lua)

##########
# Export #
##########
MYPACKAGECMAKEEXPORT()
MYPACKAGEPKGCONFIGEXPORT()

#############
# Packaging #
#############
MYPACKAGEPACK ("" "Lua 5.3.4 with unpanic support")

#########
# Setup #
#########
MYPACKAGEPRINTSETUP()
