/***********************************************************
*                                                          *
*  File : mt_matrix.h                                      *
*  Date : 13/11/2009                                       *
*  Comments : Matrix routines                              *
*                                                          *
************************************************************/

#ifndef __MT_MATRIX_H__
#define __MT_MATRIX_H__

#include "mt_common.h"
#include "mt_vector.h"

/*! \addtogroup mt */
/*! @{ */

/*! \name Matrix */
/*! @{ */

typedef mt_scalar_t mt_matrix3_t[9];
typedef mt_scalar_t mt_matrix4_t[16];

/*! \brief Copies two matrices */
void mt_mat3copy(mt_matrix3_t,const mt_matrix3_t);
/*! \brief Sets a matrix to identity*/
void mt_mat3id(mt_matrix3_t);
/*! \brief Fills a rotation matrix */
void mt_mat3rot(mt_matrix3_t,mt_scalar_t,mt_scalar_t,mt_scalar_t);
/*! \brief Fills a y-rotation matrix */
void mt_mat3yrot(mt_matrix3_t,mt_scalar_t);
/*! \brief Computes the transpose matrix */
void mt_mat3transp(mt_matrix3_t,const mt_matrix3_t);
/*! \brief Computes the absolute matrix */
void mt_mat3abs(mt_matrix3_t,const mt_matrix3_t);
/*! \brief Computes the cofactor of a matrix */
mt_scalar_t mt_mat3cofac(const mt_matrix3_t,unsigned int,unsigned int,unsigned int,unsigned int);
/*! \brief Computes the inverse matrix */
void mt_mat3inv(mt_matrix3_t,const mt_matrix3_t);
/*! \brief Multiplies two matrices */
void mt_mat3mulmat3(mt_matrix3_t,const mt_matrix3_t,const mt_matrix3_t);
/*! \brief Transforms a 3D vector */
void mt_mat3mulvec3(mt_vector3_t,const mt_matrix3_t,const mt_vector3_t);
/*! \brief Transforms a 4D vector */
void mt_mat3mulvec4(mt_vector4_t,const mt_matrix3_t,const mt_vector4_t);

/*! \brief Copies two matrices */
void mt_mat4copy(mt_matrix4_t,const mt_matrix4_t);
/*! \brief Sets a matrix to identity*/
void mt_mat4id(mt_matrix4_t);
/*! \brief Computes the transpose matrix */
void mt_mat4transp(mt_matrix4_t,const mt_matrix4_t);
/*! \brief Multiplies two matrices */
void mt_mat4mulmat4(mt_matrix4_t,const mt_matrix4_t,const mt_matrix4_t);
/*! \brief Multiplies the rotation part of two matrices */
void mt_mat4rmulmat4(mt_matrix4_t,const mt_matrix4_t,const mt_matrix4_t);
/*! \brief Transforms a 3D vector */
void mt_mat4mulvec3(mt_vector3_t,const mt_matrix4_t,const mt_vector3_t);
/*! \brief Transforms a 4D vector */
void mt_mat4mulvec4(mt_vector4_t,const mt_matrix4_t,const mt_vector4_t);
/*! \brief Transforms a 3D vector by the rotation part a the matrix */
void mt_mat4rmulvec3(mt_vector3_t,const mt_matrix4_t,const mt_vector3_t);
/*! \brief Transforms a 4D vector by the rotation part a the matrix */
void mt_mat4rmulvec4(mt_vector4_t,const mt_matrix4_t,const mt_vector4_t);
/*! \brief Inverses an affine orthogonal transformation (faster than a classical inversion) */
void mt_mat4orthoinv(mt_matrix4_t,const mt_matrix4_t);

/* @} */

/* @} */

#endif
