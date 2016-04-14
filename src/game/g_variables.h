/***********************************************************
*                                                          *
*  File : g_variables.h                                    *
*  Date : 30/12/2009                                       *
*  Comments : Global variables                             *
*                                                          *
************************************************************/

#ifndef __G_VARIABLES_H__
#define __G_VARIABLES_H__

#include "g_common.h"
#include "mt_common.h"
#include "mt_vector.h"

/*! \addtogroup g */
/*! @{ */

/*! \name Variables */
/*! @{ */

/*! Game flags */
enum G_GAME_FLAGS
{
	G_GAME_PAUSED=0x01,		/*!< The game is paused (the user interface is displayed) */
	G_GAME_CONSOLE=0x02,	/*!< The console is displayed */
	G_GAME_FADEIN=0x04		/*!< The game is fading in */
};

/*! The world gravity */
extern mt_scalar_t g_gravity;
/*! The world friction */
extern mt_scalar_t g_friction;
/*! The air friction force */
extern mt_scalar_t g_airfriction;
/*! The player step velocity */
extern mt_scalar_t g_step;
/*! The player step height */
extern mt_scalar_t g_hstep;
/*! The player jump speed */
extern mt_scalar_t g_jump;
/*! The camera distance */
extern mt_scalar_t g_distance;
/*! The character the camera looks at, or -1 */
extern int g_lookat;
/*! Camera position and center coordinates */
extern mt_vector3_t g_camera[2];
/*! The game flags */
extern int g_flags;
/*! The number of milliseconds for each frame */
extern unsigned int g_delay;

/*! \brief Resets game variables */
void g_reset(void);

/*! @} */

/*! @} */

#endif
