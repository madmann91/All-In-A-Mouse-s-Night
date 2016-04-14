/***********************************************************
*                                                          *
*  File : g_controller.h                                   *
*  Date : 24/12/2009                                       *
*  Comments : Character controllers                        *
*                                                          *
************************************************************/

#ifndef __G_CONTROLLER_H__
#define __G_CONTROLLER_H__

#include "g_common.h"
#include "mt_common.h"
#include "e_device.h"
#include "mt_vector.h"

/*! \addtogroup g */
/*! @{ */

/*! \name Controller */
/*! @{ */

#define G_MAX_FRIENDS 4

/*! Different types of controllers */
enum G_CONTROLLER_TYPES
{
	G_CONTROLLER_USER,				/*!< User control with mouse and/or keyboard */
	G_CONTROLLER_AI					/*!< AI control, the character is controlled by the computer */
};

/*! User controller keys */
enum G_CONTROL_KEYS
{
	G_CONTROL_TURNLEFT,				/*!< Turn left */
	G_CONTROL_TURNRIGHT,			/*!< Turn right */
	G_CONTROL_STRAFELEFT,			/*!< Strafe left */
	G_CONTROL_STRAFERIGHT,			/*!< Strafe right */
	G_CONTROL_FORWARD,				/*!< Go forward */
	G_CONTROL_BACKWARD,				/*!< Go backward */
	G_CONTROL_JUMP,					/*!< Jump */
	G_CONTROL_MAX
};

/*! User character controller */
struct g_usercontroller_s
{
	int controls[G_CONTROL_MAX];		/*!< Controls configuration for the character */
	mt_scalar_t sensitivity;			/*!< Mouse sensitivity */
	e_device_t* device;					/*!< The device to use to retreive input */
};

typedef struct g_usercontroller_s g_usercontroller_t;

enum G_BEHAVIOURS
{
	G_BEHAVIOUR_FLEE,		/*!< The character tries to flee */
	G_BEHAVIOUR_WAIT,		/*!< The character is careful but fights any enemy it sees */
	G_BEHAVIOUR_FIGHT		/*!< The character is seeking the enemies */
};

/*! Artificial Intelligence character controller */
struct g_aicontroller_s
{
	int behaviour;						/*!< The behaviour of the character */
	int friends[G_MAX_FRIENDS];			/*!< The class ids of its friends */
	mt_vector3_t location;				/*!< The location the character must reach */
};

typedef struct g_aicontroller_s g_aicontroller_t;

struct g_character_s;
typedef void (*g_controllerfunc_t) (struct g_character_s*,mt_scalar_t);

/*! Controller structure */
struct g_controller_s
{
	unsigned int type;			/*!< Controller type */
	union
	{
		g_usercontroller_t user;
		g_aicontroller_t ai;
	};							/*!< Controller information */
	g_controllerfunc_t func;	/*!< Controller update function */
};

typedef struct g_controller_s g_controller_t;

/*!
  \brief Creates a character controller
  \param type The type of controller
  \return The newly created controller or 0 if an error occured
  \sa G_CONTROLLER_TYPES
*/
g_controller_t* g_createcontroller(unsigned int type);
/*!
  \brief Destroys a character controller
  \param controller The controller to destroy
*/
void g_destroycontroller(g_controller_t* controller);

/*! @} */

/*! @} */

#endif
