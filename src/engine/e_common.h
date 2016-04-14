/***********************************************************
*                                                          *
*  File : e_common.h                                       *
*  Date : 13/11/2009                                       *
*  Comments : Common engine definitions                    *
*                                                          *
************************************************************/

#ifndef __E_COMMON_H__
#define __E_COMMON_H__

#include "mt_common.h"

/*! \addtogroup e Engine module */
/*! @{ */

typedef mt_scalar_t e_texcoord_t[2];
typedef mt_scalar_t e_vertex3_t[3];
typedef mt_scalar_t e_vertex4_t[4];
typedef unsigned char e_color3_t[3];
typedef unsigned char e_color4_t[4];

/*! \brief File type information */
struct e_filetype_s
{
	int magic;				/*!< Magic word identifying the file */
	int version;			/*!< Version of the file */
};

typedef struct e_filetype_s e_filetype_t;

/*! \brief Checks if the given file types are the same */
int e_checkfiletype(const e_filetype_t*,const e_filetype_t*);

/*! @} */

#endif
