/***********************************************************
*                                                          *
*  File : mt_transform.h                                   *
*  Date : 13/11/2009                                       *
*  Comments : Object transformation routines               *
*                                                          *
************************************************************/

#ifndef __MT_TRANSFORM_H__
#define __MT_TRANSFORM_H__

#include "mt_common.h"
#include "mt_vector.h"
#include "mt_matrix.h"

/*! \addtogroup mt */
/*! @{ */

/*! \name Transform */
/*! @{ */

/*! Transformation structure */
struct mt_transform_s
{
	mt_matrix3_t rotation;		/*!< Rotation matrix to apply */
	mt_vector3_t position;		/*!< Position in world space */
};

typedef struct mt_transform_s mt_transform_t;

/*! \brief Transform a vertex given a transformation */
void mt_transformvector(mt_vector3_t,const mt_vector3_t,const mt_transform_t*);
/*! \brief Transform an axis-aligned box given a transformation */
void mt_transformbox(mt_vector3_t,const mt_vector3_t,const mt_transform_t*);
/*! \brief Computes the inverse transformation */
void mt_inversetransform(mt_transform_t*,const mt_transform_t*);
/*! \brief Fills a 4x4 matrix to use with OpenGL */
void mt_getmatrix(mt_matrix4_t,const mt_transform_t*);

/*! @} */

/*! @} */

#endif
