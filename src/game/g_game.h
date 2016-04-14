/***********************************************************
*                                                          *
*  File : g_game.h                                         *
*  Date : 13/11/2009                                       *
*  Comments : The game itself                              *
*                                                          *
************************************************************/

#ifndef __G_GAME_H__
#define __G_GAME_H__

#include "mt_common.h"

#include "e_device.h"
#include "e_camera.h"
#include "e_model.h"
#include "e_texture.h"
#include "e_font.h"
#include "e_particles.h"
#include "e_utils.h"

#include "g_common.h"
#include "g_world.h"
#include "g_script.h"

/*! \addtogroup g */
/*! @{ */

/*! \name Game */
/*! @{ */

/*! Fading delay (in milliseconds) */
#define G_FADE_DELAY 7000

enum G_FONT_TYPES
{
	G_FONT_SMALL,
	G_FONT_MEDIUM,
	G_FONT_BIG,
	G_FONT_MAX
};

struct g_game_s
{
	//Game objects
	g_world_t* world;
	g_script_t* script;

	//Engine objects
	e_texture_t* iconset;
	e_font_t* fonts[G_FONT_MAX];
	e_camera_t camera;
	e_particles_t** particles;
	unsigned int numparticles;

	//Timer object (various uses)
	e_timer_t timer;

	//The description of the game
	char* title;
	char** description;
	unsigned int numlines;
	unsigned int maxwidth;
};

typedef struct g_game_s g_game_t;

/*!
  \brief Loads the specified game package
  \param[in] device The device to use
  \param[in] name The game name
  \return 0 if an error occured, otherwise 1
*/
int g_loadgame(e_device_t* device,const char* name);
/*!
  \brief Updates the current game
  \param[in] device The device to use
  \param[in] dt Time step
*/
void g_updategame(e_device_t* device,mt_scalar_t dt);
/*!
  \brief Renders the current game
  \param[in] device The device to use
*/
void g_rendergame(e_device_t* device);
/*! \brief Destroys the current game */
void g_destroygame(void);

/*! @} */

/*! @} */

#endif
