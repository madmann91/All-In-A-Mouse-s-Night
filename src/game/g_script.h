/***********************************************************
*                                                          *
*  File : g_script.h                                       *
*  Date : 13/11/2009                                       *
*  Comments : Interaction with lua scripts                 *
*                                                          *
************************************************************/

#ifndef __G_SCRIPT_H__
#define __G_SCRIPT_H__

#include "g_common.h"
#include "g_world.h"

#include "fs_common.h"

/*! \addtogroup g */
/*! @{ */

/*! \name Script */
/*! @{ */

struct g_script_s
{
	lua_State* state;
	g_world_t* world;
	e_device_t* device;
};

typedef struct g_script_s g_script_t;

/*!
  \brief Initializes game scripts
  \param[in] device A pointer to the device object
  \param[in] world A pointer to the world object
  \param[in] file The file that contains the script text
  \return A valid script object, or 0 if an error occured
*/
g_script_t* g_loadscript(e_device_t* device,g_world_t* world,fs_handle_t file);
/*!
  \brief Calls a script function
  \param[in] script A script object
  \param[in] name The function name
  \return 1 if no error occured, otherwise 0
*/
int g_callscript(g_script_t* script,const char* name);
/*!
  \brief Destroys the given script
  \param[in] script A script object
*/
void g_destroyscript(g_script_t* script);

//Game script functions
//These functions are only available in game scripts, and should not be called directly.
//The '__' prefix is meant to prevent conflicts with existing functions

/*! \brief g_loadlibrary(library) */
int __g_loadlibrary(lua_State* state);
/*! \brief g_registerweapon(class,effect,model,texture) */
int __g_registerweapon(lua_State* state);
/*! \brief g_addweapon(class,character) */
int __g_addweapon(lua_State* state);
/*! \brief g_registercharacter(class,mass,life,aiclass,model,skeleton,texture) */
int __g_registercharacter(lua_State* state);
/*! \brief g_addcharacter(class,name,position,controller) */
int __g_addcharacter(lua_State*);
/*! \brief g_removecharacter(name) */
int __g_removecharacter(lua_State*);
/*! \brief g_lookat(name,distance) or g_lookat(position,center) */
int __g_lookat(lua_State*);
/*! \brief g_physics(gravity,friction,airfriction,step,jump) */
int __g_physics(lua_State*);
/*! \brief g_print(...) */
int __g_print(lua_State*);

/*! @} */

/*! @} */

#endif
