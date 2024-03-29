#
# Multi-config generator, e.g. Visual Studio on Windows:
#
# cmake -S c-luaunpanic -B c-luaunpanic-build
# cmake --build c-luaunpanic-build --config RelWithDebInfo
# ctest --test-dir c-luaunpanic-build -C RelWithDebInfo
# Windows:
#   cmake --install c-luaunpanic-build --config RelWithDebInfo --prefix %cd%/c-luaunpanic-install
# Others:
#   cmake --install c-luaunpanic-build --config RelWithDebInfo --prefix `pwd`/c-luaunpanic-install
# cmake --build c-luaunpanic-build --config RelWithDebInfo --target package
#
# Single-config generator, e.g. NMake Makefiles on Windows, Unix Makefiles on Linxu:
#
# cmake -S c-luaunpanic -B c-luaunpanic-build -DCMAKE_BUILD_TYPE=RelWithDebInfo
# cmake --build c-luaunpanic-build
# ctest --test-dir c-luaunpanic-build
# Windows:
#   cmake --install c-luaunpanic-build --prefix %cd%/c-luaunpanic-install
# Others:
#   cmake --install c-luaunpanic-build --prefix `pwd`/c-luaunpanic-install
# cmake --build c-luaunpanic-build --target package
#
# Local tests done with: cmake -S c-luaunpanic -B c-luaunpanic-build -DCMAKE_HELPERS_DEBUG=OFF -DICU_ROOT=C:\icu4c-74_2-Win64-MSVC2019
#
cmake_minimum_required(VERSION 3.26.0 FATAL_ERROR)
project(luaunpanic VERSION 5.3.4 LANGUAGES C)
#
# Specific options
#
option(LUA_32BITS "Use 32-bit integers and 32-bit floats" OFF)
option(LUA_USE_C89 "Avoid the use of few C99 feature" OFF)
#
# Get library helper
#
include(FetchContent)
if("x$ENV{CMAKE_HELPERS_DEPEND_CMAKE_HELPERS_FILE}" STREQUAL "x")
  FetchContent_Declare(cmake-helpers GIT_REPOSITORY https://github.com/jddurand/cmake-helpers.git GIT_SHALLOW TRUE)
else()
  FetchContent_Declare(cmake-helpers URL $ENV{CMAKE_HELPERS_DEPEND_CMAKE_HELPERS_FILE})
endif()
FetchContent_MakeAvailable(cmake-helpers)
#
# Create library
#
cmake_helpers_library(luaunpanic
  CONFIG_ARGS                    include/luaunpanic/internal/luaconf.h.in include/luaunpanic/lua/luaconf.h
  PODS_AUTO                      FALSE
  SOURCES_ACCEPT_RELPATH_REGEXES luaunpanic_amalgamation.c
  TARGETS_OUTVAR                 targets
)
#
# OS-specifics
#
foreach(_target IN LISTS targets)
  if(WIN32 AND NOT CYGWIN)
    target_compile_definitions(${_target} PUBLIC -DLUA_DL_DLL)
  else()
    target_compile_definitions(${_target} PUBLIC -DLUA_USE_DLOPEN -DLUA_USE_POSIX)
  endif()
endforeach()
#
# We moved lua headers to include/luaunpanic/lua
#
foreach(_target IN LISTS targets)
  target_include_directories(${_target} PUBLIC $<BUILD_LOCAL_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/luaunpanic/lua>)
  target_include_directories(${_target} PUBLIC $<BUILD_LOCAL_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/include/luaunpanic/lua>)
endforeach()
#
# dl and m dependency
#
if(CMAKE_DL_LIBS OR CMAKE_MATH_LIBS)
  foreach(_target IN LISTS targets)
    if(CMAKE_DL_LIBS)
      target_link_libraries(${_target} PUBLIC ${CMAKE_DL_LIBS})
    endif()
    if(CMAKE_MATH_LIBS)
      target_link_libraries(${_target} PUBLIC ${CMAKE_MATH_LIBS})
    endif()
  endforeach()
endif()
#
# Those definitions have to propagate to those using the library
#
set(lua_public_cflags)
if(LUA_32BITS)
  list(APPEND lua_public_cflags "-DLUA_32BITS")
endif()
if(LUA_USE_C89)
  list(APPEND lua_public_cflags "-DLUA_USE_C89")
endif()
if(lua_public_cflags)
  foreach(_target IN LISTS targets)
    target_compile_definitions(${_target} PUBLIC ${lua_public_cflags})
  endforeach()
endif()
#
# Those definitions are private
#
if(CMAKE_C_COMPILER_ID STREQUAL SunPro)
  foreach(_target IN LISTS targets)
    target_compile_options(${_target} PRIVATE -xc99)
  endforeach()
endif()
#
# EXEs
#
include(CTest)
cmake_helpers_exe(luaunpanicwrapperTester
  SOURCES test/luawrapperTester.c
  INSTALL FALSE
  TEST TRUE
  TEST_ARGS ${CMAKE_CURRENT_SOURCE_DIR}/test/luawrapperTester.lua
)
cmake_helpers_exe(luaunpanicwrapperTester2
  SOURCES test/luawrapperTester2.c
  INSTALL FALSE
  TEST TRUE
  TEST_ARGS ${CMAKE_CURRENT_SOURCE_DIR}/test/luawrapperTester2.lua
)
#
# Package
#
cmake_helpers_package(
  EXTRA_LICENSES Lua ${CMAKE_CURRENT_SOURCE_DIR}/src/LICENSE
)
