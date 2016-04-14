/***********************************************************
*                                                          *
*  File : mt_vector.h                                      *
*  Date : 20/02/2011                                       *
*  Comments : Vector operations                            *
*                                                          *
************************************************************/

#ifndef __MT_VECTOR_H__
#define __MT_VECTOR_H__

#include "mt_common.h"

/*! \addtogroup mt */
/*! @{ */

/*! \name Vector */
/*! @{ */ 

typedef mt_scalar_t mt_vector3_t[3];
typedef mt_scalar_t mt_vector4_t[3];

/*! \brief Sets the vector values */
void mt_vec3set(mt_vector3_t,mt_scalar_t,mt_scalar_t,mt_scalar_t);
/*! \brief Copies two vectors */
void mt_vec3copy(mt_vector3_t,const mt_vector3_t);
/*! \brief Sets a vector to zero */
void mt_vec3zero(mt_vector3_t);
/*! \brief Normalizes a vector */
void mt_vec3norm(mt_vector3_t);
/*! \brief Computes the length of the given vector */
mt_scalar_t mt_vec3len(const mt_vector3_t);
/*! \brief Computes the square length of the given vector */
mt_scalar_t mt_vec3len2(const mt_vector3_t);
/*! \brief Finds the minimum coordinates of two vectors */
void mt_vec3min(mt_vector3_t,const mt_vector3_t);
/*! \brief Finds the maximum coordinates of two vectors */
void mt_vec3max(mt_vector3_t,const mt_vector3_t);
/*! \brief Computes the difference of two vectors */
void mt_vec3diff(const mt_vector3_t,const mt_vector3_t,mt_vector3_t);
/*! \brief Computes the sum of two vectors */
void mt_vec3sum(const mt_vector3_t,const mt_vector3_t,mt_vector3_t);
/*! \brief Computes the opposite of a vector */
void mt_vec3neg(mt_vector3_t);
/*! \brief Multiplies the coordinates of a vector by a given scalar */
void mt_vec3scale(mt_vector3_t,mt_scalar_t);
/*! \brief Multiplies the coordinates of a vector by a given vector */
void mt_vec3mulvec3(mt_vector3_t,const mt_vector3_t);
/*! \brief Computes the dot product of two vectors */
mt_scalar_t mt_vec3dot(const mt_vector3_t,const mt_vector3_t);
/*! \brief Computes the cross product of two vectors */
void mt_vec3cross(const mt_vector3_t,const mt_vector3_t,mt_vector3_t);

/*! \brief Sets the vector values */
void mt_vec4set(mt_vector4_t,mt_scalar_t,mt_scalar_t,mt_scalar_t,mt_scalar_t);
/*! \brief Copies two vectors */
void mt_vec4copy(mt_vector4_t,const mt_vector4_t);
/*! \brief Sets a vector to zero */
void mt_vec4zero(mt_vector4_t);
/*! \brief Normalizes a vector */
void mt_vec4norm(mt_vector4_t);
/*! \brief Computes the length of the given vector */
mt_scalar_t mt_vec4len(const mt_vector4_t);
/*! \brief Computes the square length of the given vector */
mt_scalar_t mt_vec4len2(const mt_vector4_t);
/*! \brief Finds the minimum coordinates of two vectors */
void mt_vec4min(mt_vector3_t,const mt_vector4_t);
/*! \brief Finds the maximum coordinates of two vectors */
void mt_vec4max(mt_vector3_t,const mt_vector4_t);
/*! \brief Computes the difference of two vectors */
void mt_vec4diff(const mt_vector4_t,const mt_vector4_t,mt_vector4_t);
/*! \brief Computes the sum of two vectors */
void mt_vec4sum(const mt_vector4_t,const mt_vector4_t,mt_vector4_t);
/*! \brief Computes the opposite of a vector */
void mt_vec4neg(mt_vector4_t);
/*! \brief Multiplies the coordinates of a vector by a given scalar */
void mt_vec4scale(mt_vector4_t,mt_scalar_t);
/*! \brief Multiplies the coordinates of a vector by a given vector */
void mt_vec4mulvec4(mt_vector4_t,const mt_vector4_t);
/*! \brief Computes the dot product of two vectors */
mt_scalar_t mt_vec4dot(const mt_vector4_t,const mt_vector4_t);

/* @} */

/* @} */

#endif
