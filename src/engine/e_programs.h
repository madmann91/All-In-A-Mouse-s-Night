/***********************************************************
*                                                          *
*  File : e_programs.h                                     *
*  Date : 13/11/2009                                       *
*  Comments : Vertex programs                              *
*                                                          *
************************************************************/

#ifndef __E_PROGRAMS_H__
#define __E_PROGRAMS_H__

#include "e_common.h"

/*! \addtogroup e */
/*! @{ */

/*! \name Programs */
/*! @{ */

/*! Vertex programs */
enum E_PROGRAMS
{
	E_PROGRAM_PASSTOFRAGMENT,	/*!< Vertex normal and position passed as additional texture coordinates */
	E_PROGRAM_NOSHADING,		/*!< "No shading" fragment program */
	E_PROGRAM_CELSHADING,		/*!< Cel-shading with one light source */
	E_PROGRAM_SHADOW,			/*!< Shadow volume extrusion */
	E_MAX_PROGRAMS
};

extern unsigned int e_programids[E_MAX_PROGRAMS];

/*!
  \brief Registers vertex programs
  The e_createdevice function automatically calls this function, so you should not try to call it directly.
*/
void e_registerprograms(void);
/*!
  \brief Unregisters vertex programs
  The e_destroydevice function automatically calls this function, so you should not try to call it directly.
*/
void e_unregisterprograms(void);

/*! @} */

/*! @} */

#endif
