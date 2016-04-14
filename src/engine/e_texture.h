/***********************************************************
*                                                          *
*  File : e_texture.h                                      *
*  Date : 13/11/2009                                       *
*  Comments : Texture routines                             *
*                                                          *
************************************************************/

#ifndef __E_TEXTURE_H__
#define __E_TEXTURE_H__

#include "e_common.h"
#include "e_device.h"

#include "fs_common.h"

/*! \addtogroup e */
/*! @{ */

/*! \name Texture */
/*! @{ */

struct e_texture_s
{
	GLuint id;
	unsigned int width;
	unsigned int height;
	unsigned int bpp;
};

typedef struct e_texture_s e_texture_t;

/*!
  \brief Loads a texture (only PNG or JPG files supported)
  \param[in] device The device to use for loading
  \param[in] mipmaps Set this parameter to 1 if you want mipmaps
  \param[in] file The file to read data from
  \return The loaded texture or 0 if an error occured
*/
e_texture_t* e_loadtexture(e_device_t* device,int mipmaps,fs_handle_t file);
/*!
  \brief Creates a gradient texture
  \param[in] device The device to use
  \param[in] values The gradient values
  \param[in] numvalues The number of values
  \return The newly created gradient texture or 0 if an error occured
*/
e_texture_t* e_creategradient(e_device_t* device,const unsigned char* values,unsigned int numvalues);
/*!
  \brief Destroys a texture
  \param[in] texture The texture to destroy
*/
void e_destroytexture(e_texture_t* texture);
/*!
  \brief Renders a texture on screen
  \param[in] device The device to render the texture to
  \param[in] texture The texture object
  \param[in] xoffset Horizontal offset in icon set
  \param[in] yoffset Vertical offset in icon set
  \param[in] width Icon width
  \param[in] height Icon height
  \param[in] x Horizontal position
  \param[in] y Vertical position
*/
void e_rendertexture(e_device_t* device,e_texture_t* texture,int xoffset,int yoffset,unsigned int width,unsigned int height,int x,int y);

/*! @} */

/*! @} */

#endif
