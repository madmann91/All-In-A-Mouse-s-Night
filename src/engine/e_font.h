/***********************************************************
*                                                          *
*  File : e_font.h                                         *
*  Date : 13/11/2009                                       *
*  Comments : Font loading and rendering                   *
*                                                          *
************************************************************/

#ifndef __E_FONT_H__
#define __E_FONT_H__

#include "config.h"

#include "e_common.h"
#include "e_device.h"

#include "fs_common.h"

/*! \addtogroup e */
/*! @{ */

/*! \name Font */
/*! @{ */

#define E_FONT_CHARACTERS 255

struct e_fontcharacter_s
{
	int top;
	int left;
	unsigned int width;
	unsigned int height;
	unsigned int advance;
};

typedef struct e_fontcharacter_s e_fontcharacter_t;

struct e_font_s
{
	GLuint base;
	GLuint textures[E_FONT_CHARACTERS];
	e_fontcharacter_t characters[E_FONT_CHARACTERS];
	unsigned int maxheight;
	unsigned int maxwidth;
};

typedef struct e_font_s e_font_t;

/*!
  \brief Loads a font
  \param[in] device The device to use for loading
  \param[in] size The desired font size
  \param[in] file The file to read data from
  \return 0 or the loaded font
*/
e_font_t* e_loadfont(e_device_t* device,unsigned int size,fs_handle_t file);
/*!
  \brief Destroys a font
  \param[in] font The font to destroy
*/
void e_destroyfont(e_font_t* font);
/*!
  \brief Gets the rendered text size
  \param[in] font The font to use
  \param[in] text The text
  \param[out] width The text width
  \param[out] height The text height
*/
void e_textsize(const e_font_t* font,const char* text,unsigned int* width,unsigned int* height);
/*!
  \brief Gets the rendered text size within a range
  \param[in] font The font to use
  \param[in] text The text
  \param[in] count The number of characters to consider
  \param[out] width The text width
  \param[out] height The text height
*/
void e_textsizerange(const e_font_t* font,const char* text,unsigned int count,unsigned int* width,unsigned int* height);
/*!
  \brief Print text on screen
  \param[in] device The device
  \param[in] font The font to use
  \param[in] text The text to print
  \param[in] x Horizontal position
  \param[in] y Vertical position
*/
void e_print(e_device_t* device,const e_font_t* font,int x,int y,const char* text);
/*!
  \brief Print text on screen within a range
  \param[in] device The device
  \param[in] font The font to use
  \param[in] text The text to print
  \param[in] count The number of characters to render
  \param[in] x Horizontal position
  \param[in] y Vertical position
*/
void e_printrange(e_device_t* device,const e_font_t* font,int x,int y,const char* text,unsigned int count);

/*! @} */

/*! @} */

#endif
