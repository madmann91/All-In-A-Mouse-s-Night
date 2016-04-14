/***********************************************************
*                                                          *
*  File : mt_collision.h                                   *
*  Date : 13/11/2009                                       *
*  Comments : Collision routines                           *
*                                                          *
************************************************************/

#ifndef __MT_COLLISION_H__
#define __MT_COLLISION_H__

#include "mt_common.h"
#include "mt_vector.h"
#include "mt_plane.h"

/*! \addtogroup mt */
/*! @{ */

/*!
  \name Collision
  This section implements several state-of-the-art collision detection algorithms.
  Most of them have been adapted from the <em>Journal Of Graphic Tools</em>.
*/
/*! @{ */

/*
  \brief Frustum to box collision
  \param[in] min The minimum dimensions of the box
  \param[in] max The maximum dimensions of the box
  \param[in] frustum The frustum planes
  \return 1 if a collision was found, otherwise 0
*/
int mt_frustumtobox(const mt_plane_t frustum[6],const mt_vector3_t min,const mt_vector3_t max);
/*!
  \brief Ray to triangle collision
  \param[in] start Ray starting point
  \param[in] direction Ray direction
  \param[in] a The first triangle point
  \param[in] b The second triangle point
  \param[in] c The third triangle point
  \param[out] t0 Collision depth
  \return 1 if a collision was found
*/
int mt_raytotriangle(const mt_vector3_t start,const mt_vector3_t direction,const mt_vector3_t a,const mt_vector3_t b,const mt_vector3_t c,mt_scalar_t* t0);
/*!
  \brief Ray to box collision
  \param[in] start Ray starting point
  \param[in] invdir Ray inverse direction (ie 1.0/dir)
  \param[in] min Box minimum dimensions
  \param[in] max Box maximum dimensions
  \param[out] t0 First collision bound
  \param[out] t1 Second collision bound
  \return 1 if a collision was found
*/
int mt_raytobox(const mt_vector3_t start,const mt_vector3_t invdir,const mt_vector3_t min,const mt_vector3_t max,mt_scalar_t* t0,mt_scalar_t* t1);
/*!
  \brief Triangle to box collision
  \param[in] a The triangle first point
  \param[in] b The triangle second point
  \param[in] c The triangle third point
  \param[in] halfextents Box half extents (The box center is the origin)
  \return 1 if a collision was found
*/
int mt_triangletobox(const mt_vector3_t a,const mt_vector3_t b,const mt_vector3_t c,const mt_vector3_t halfextents);
/*!
  \brief Moving triangle to box collision (sweep test)
  \param[in] a The triangle first point
  \param[in] b The triangle second point
  \param[in] c The triangle third point
  \param[in] velocity The velocity of the triangle
  \param[in] halfextents Box half extents (The box center is the origin)
  \param[out] t0 First time of intersection (between 0.0 and 1.0)
  \param[out] n The collision normal (if any)
  \return 1 if a collision was found
*/
int mt_sweeptriangletobox(const mt_vector3_t a,const mt_vector3_t b,const mt_vector3_t c,const mt_vector3_t velocity,const mt_vector3_t halfextents,mt_scalar_t* t0,mt_vector3_t n);
/*!
  \brief Moving box to box collision (sweep test)
  \param[in] min1 The minimum dimensions of the first box
  \param[in] max1 The maximum dimensions of the first box
  \param[in] invvelocity The inverse velocity of the first box
  \param[in] min2 The minimum dimensions of the second box
  \param[in] max2 The maximum dimensions of the second box
  \param[out] t0 First time of intersection (between -MT_MAXIMUM and +MT_MAXIMUM)
  \param[out] t1 Last time of intersection (between -MT_MAXIMUM and +MT_MAXIMUM)
  \param[out] n The collision normal (if any)
  \return 1 if a collision was found
*/
int mt_sweepboxtobox(const mt_vector3_t min1,const mt_vector3_t max1,const mt_vector3_t invvelocity,const mt_vector3_t min2,const mt_vector3_t max2,mt_scalar_t* t0,mt_scalar_t* t1,mt_vector3_t n);
/*!
  \brief Moving sphere to triangle collision (sweep test)
  \param[in] velocity The velocity of the sphere
  \param[in] position The position of the sphere
  \param[in] a The triangle first point
  \param[in] b The triangle second point
  \param[in] c The triangle third point
  \param[out] t0 First time of intersection (between 0.0 and 1.0)
  \param[out] n The collision normal (if any)
  \return 1 if a face-collision ocurred, 2 if an edge-collision occured, and 0 if no collision was found
*/
int mt_sweepspheretotriangle(const mt_vector3_t position,const mt_vector3_t velocity,const mt_vector3_t a,const mt_vector3_t b,const mt_vector3_t c,mt_scalar_t* t0,mt_vector3_t n);

/*! @} */

/*! @} */

#endif
