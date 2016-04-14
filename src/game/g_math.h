/***********************************************************
*                                                          *
*  File : g_math.h                                         *
*  Date : 30/12/2009                                       *
*  Comments : Some helpful mathematic routines             *
*                                                          *
************************************************************/

#ifndef __G_MATH_H__
#define __G_MATH_H__

#include "g_common.h"
#include "g_character.h"
#include "mt_vector.h"
#include "mt_mesh.h"
#include "mt_divider.h"

/*! \addtogroup g */
/*! @{ */

/*! \name Math */
/*! @{ */

/*! The maximum number of iterations necessary to compute the new character position */
#define G_ITERATION_COUNT 4
/*! The maximum height ratio under which the player is considered as 'on the ground' */
#define G_HEIGHT_LIMIT 0.1
/*! The collision margin (ie the minimum distance between the player and the world) */
#define G_COLLISION_MARGIN 0.001
/*! The collision threshold (value used to determine if the direction is parallel to a triangle, see code for details) */
#define G_COLLISION_THRESHOLD -0.001
/*! The camera threshold (ie the minimum camera-player distance) */
#define G_CAMERA_THRESHOLD 1.5
/*! The camera distance (ie the distance from the eye to the object) */
#define G_CAMERA_DISTANCE 100.0

/*!
  \brief Sets the character flags
  \param[in] character The character whose flags are to be updated
  \param[in] index The collider index associated with the character
  \param[in] divider The divider that contains every character
  \param[in] mesh The mesh containing triangle data
*/
void g_characterflags(g_character_t* character,unsigned int index,mt_divider_t* divider,const mt_mesh_t* mesh);
/*!
  \brief Handles character collisions
  \param[in] character The character to update
  \param[in] index The collider index associated with the character
  \param[in] divider The divider that contains every character
  \param[in] mesh The mesh containing triangle data
  \param[in] dt Time step
*/
void g_charactermove(g_character_t* character,unsigned int index,mt_divider_t* divider,const mt_mesh_t* mesh,mt_scalar_t dt);
/*!
  \brief Finds the new camera position
  \param[out] position The camera position
  \param[out] destination The point the camera looks at
  \param[in] character The character to use
  \param[in] mesh The mesh containing triangle data
*/
void g_charactercamera(mt_vector3_t position,mt_vector3_t destination,const g_character_t* character,const mt_mesh_t* mesh);

/*! @} */

/*! @} */

#endif
