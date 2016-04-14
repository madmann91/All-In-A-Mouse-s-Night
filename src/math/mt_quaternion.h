/***********************************************************
*                                                          *
*  File : mt_quaternion.h                                  *
*  Date : 20/02/2011                                       *
*  Comments : Vector operations                            *
*                                                          *
************************************************************/

#ifndef __MT_QUATERNION_H__
#define __MT_QUATERNION_H__

#include "mt_common.h"
#include "mt_vector.h"
#include "mt_matrix.h"

/*! \addtogroup mt */
/*! @{ */

/*! \name Quaternion */
/*! @{ */

typedef mt_scalar_t mt_quaternion_t[4];

/*! \brief Sets the quaternion values */
void mt_quatset(mt_quaternion_t,mt_scalar_t,mt_scalar_t,mt_scalar_t,mt_scalar_t);
/*! \brief Copies two quaternions */
void mt_quatcopy(mt_quaternion_t,const mt_quaternion_t);
/*! \brief Sets a quaternion to zero */
void mt_quatzero(mt_quaternion_t);
/*! \brief Normalizes a quaternion */
void mt_quatnorm(mt_quaternion_t);
/*! \brief Computes the length of the given quaternion */
mt_scalar_t mt_quatlen(const mt_quaternion_t);
/*! \brief Computes the square length of the given quaternion */
mt_scalar_t mt_quatlen2(const mt_quaternion_t);
/*! \brief Computes the inverse of a quaternion */
void mt_quatinv(mt_quaternion_t,const mt_quaternion_t);
/*! \brief Computes the orientation of a quaternion */
void mt_quatori(mt_vector3_t,const mt_quaternion_t);
/*! \brief Computes the quaternion corresponding to the given rotation */
void mt_quatrot(mt_quaternion_t,const mt_vector3_t);
/*! \brief Computes the spherical interpolation between two quaternions */
void mt_quatslerp(mt_quaternion_t,const mt_quaternion_t,const mt_quaternion_t,mt_scalar_t);
/*! \brief Computes the normal interpolation between two quaternions */
void mt_quatnlerp(mt_quaternion_t,const mt_quaternion_t,const mt_quaternion_t,mt_scalar_t);
/*! \brief Multiplies the coordinates of a quaternion by a given scalar */
void mt_quatscale(mt_quaternion_t,mt_scalar_t);
/*! \brief Multiplies two quaternions */
void mt_quatmulquat(mt_quaternion_t,const mt_quaternion_t,const mt_quaternion_t);
/*! \brief Multiplies the inverse of the first quaternion by the second */
void mt_quatimulquat(mt_quaternion_t,const mt_quaternion_t,const mt_quaternion_t);
/*! \brief Multiplies a quaternion by a given vector */
void mt_quatmulvec3(mt_vector3_t,const mt_quaternion_t,const mt_vector3_t);
/*! \brief Multiplies the inverse of a quaternion by a given vector */
void mt_quatimulvec3(mt_vector3_t,const mt_quaternion_t,const mt_vector3_t);
/*! \brief Computes the dot product of two quaternions */
mt_scalar_t mt_quatdot(const mt_quaternion_t,const mt_quaternion_t);
/*! \brief Converts the given quaternion to a rotation matrix */
void mt_quattomat3(mt_matrix3_t,const mt_quaternion_t);
/*! \brief Computes a 4*4 matrix from a quaternion and a translation vector */
void mt_quattomat4(mt_matrix4_t,const mt_quaternion_t,const mt_vector3_t);

/* @} */

/* @} */

#endif
