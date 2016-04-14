/***********************************************************
*                                                          *
*  File : e_blur.h                                         *
*  Date : 13/11/2009                                       *
*  Comments : Blur effect                                  *
*                                                          *
************************************************************/

#ifndef __E_EFFECTS_H__
#define __E_EFFECTS_H__

#include "e_device.h"

/*! \addtogroup e */
/*! @{ */

/*! \name Blur */
/*! @{ */

/*! A blur effect structure */
struct e_blur_s
{
	unsigned int texture;
	unsigned int size[2];
	e_color3_t color;
	unsigned int count;
};

typedef struct e_blur_s e_blur_t;

/*!
  \brief Creates a blur effect
  \param[in] device The device to use
  \param[in] color The color of the blur effect
  \param[in] count The number of iterations
*/
e_blur_t* e_createblur(e_device_t* device,e_color3_t color,unsigned int count);
/*! \brief Starts a blur-like effect */
void e_beginblur(e_blur_t* blur,e_device_t* device);
/*! \brief Renders a blur-like effect */
void e_endblur(e_blur_t* blur,e_device_t* device);
/*! \brief Destroys a blur effect */
void e_destroyblur(e_blur_t* blur);

/*! @} */

/*! @} */

#endif
