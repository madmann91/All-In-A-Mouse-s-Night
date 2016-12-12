/***********************************************************
*                                                          *
*  File : config.h                                         *
*  Date : 13/11/2009                                       *
*  Comments : Configuration file for the whole project      *
*                                                          *
************************************************************/

#ifndef __CONFIG_H__
#define __CONFIG_H__

/*!
  \mainpage
  \section Introduction

  All In A  is Mouse's Night is a Third Person Shooter project based on TheDreamMachine engine.
  It is designed to work on Windows, Mac and Linux Platforms.

  \section Modules
  There are currently five modules :
  \li System Module, handles OS-specific issues and provides a file system
  \li Engine Module, a module designed to help rendering models and maps
  \li Mathematics Module, mainly for physics and collision detection
  \li User Interface Module, designed to render user-friendly interfaces
  \li Game Module, the game itself, built on top of the other modules
*/

//Standard headers
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <float.h>
#include <time.h>
#include <assert.h>

#ifdef WIN32
//Windows stuff (needed by gl.h and file system wrapper)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <wchar.h>
#include <io.h>
#else
//Required for POSIX file system wrapper
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#endif

//OpenGL context
#include <SDL.h>

//Compression
#include <zlib.h>

//OpenGL headers
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glext.h>

//Lua Scripting
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>

//Image loading
#include <png.h>
#include <jpeglib.h>

//Font loading
#include <ft2build.h>
#include <freetype/freetype.h>
#include <freetype/ftglyph.h>
#include <freetype/ftoutln.h>
#include <freetype/fttrigon.h>

#endif
