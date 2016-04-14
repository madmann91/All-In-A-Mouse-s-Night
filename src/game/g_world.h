/***********************************************************
*                                                          *
*  File : g_world.h                                        *
*  Date : 13/11/2009                                       *
*  Comments : Loads a world and displays it                *
*                                                          *
************************************************************/

#ifndef __G_WORLD_H__
#define __G_WORLD_H__

#include "g_common.h"
#include "g_character.h"
#include "e_device.h"
#include "e_map.h"
#include "mt_mesh.h"
#include "mt_divider.h"

/*! \addtogroup g */
/*! @{ */

/*! \name World */
/*! @{ */

/*! The world margin that prevents any problem when a player is close to the boundaries of the map */
#define G_WORLD_MARGIN 5
/*! The name of the map filename in each world package */
#define G_MAP_FILENAME "world.map"

/*! The world structure */
struct g_world_s
{
	e_map_t* map;						/*!< A 3D map */
	unsigned int numtextures;			/*!< The number of textures needed for the map */
	e_texture_t** textures;				/*!< An array of textures */
	e_texture_t* gradient;				/*!< The lighting gradient */

	mt_mesh_t* mesh;					/*!< The mesh structure associated with the map */
	mt_divider_t* divider;				/*!< The divider structure */

	g_character_t* characters;			/*!< A pointer to the characters present in the game */
	unsigned int numcharacters;			/*!< The number of characters */
	g_weapon_t* weapons;				/*!< A pointer to the weapons available in the game */
	unsigned int numweapons;			/*!< The number of weapons */

	g_characterclass_t* charactertypes;	/*!< A pointer to the character classes */
	unsigned int numcharactertypes;		/*!< The number of character classes */
};

typedef struct g_world_s g_world_t;

/*!
  \brief Loads a world from a package
  \param[in] device The device to use
  \param[in] world The world directory name (can be a package name too)
  \return A valid world object or 0 if an error occured
*/
g_world_t* g_loadworld(e_device_t* device,const char* world);
/*!
  \brief Updates the world (update physics and ai)
  \param[in] world The world to update
  \param[in] dt Time step
*/
void g_updateworld(g_world_t* world,mt_scalar_t dt);
/*!
  \brief Renders the world
  \param[in] device The device to use
  \param[in] world The world to render
  \param[in] camera The camera (for frustum culling)
*/
void g_renderworld(e_device_t* device,const g_world_t* world,const e_camera_t* camera);
/*!
  \brief Destroys the given world
  \param[in] world The world to destroy
*/
void g_destroyworld(g_world_t* world);

/*! @} */

/*! @} */

#endif
