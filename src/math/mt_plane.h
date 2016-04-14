/***********************************************************
*                                                          *
*  File : mt_plane.h                                       *
*  Date : 13/11/2009                                       *
*  Comments : Plane routines                               *
*                                                          *
************************************************************/

#ifndef __MT_PLANE_H__
#define __MT_PLANE_H__

#include "mt_common.h"
#include "mt_vector.h"

/*! \addtogroup mt */
/*! @{ */

/*! \name Plane */
/*! @{ */

/*! Plane structure */
typedef mt_scalar_t mt_plane_t[4];

/*! \brief Sets the plane given a normal vector and a distance */
void mt_planeset(mt_plane_t,const mt_vector3_t,mt_scalar_t);
/*! \brief Copies two planes */
void mt_planecopy(mt_plane_t,const mt_plane_t);
/*! \brief Returns the plane distance to a point */
mt_scalar_t mt_planedistance(const mt_plane_t,const mt_vector3_t);
/*! \brief Computes the plane containing a triangle */
void mt_triangleplane(const mt_vector3_t,const mt_vector3_t,const mt_vector3_t,mt_plane_t);
/*! \brief Computes a triangle normal */
void mt_trianglenormal(const mt_vector3_t,const mt_vector3_t,const mt_vector3_t,mt_vector3_t);

/*! @} */

/*! @} */

#endif

