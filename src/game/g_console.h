/***********************************************************
*                                                          *
*  File : g_console.h                                      *
*  Date : 13/03/2011                                       *
*  Comments : In-game console and error logging            *
*                                                          *
************************************************************/

#ifndef __G_CONSOLE_H__
#define __G_CONSOLE_H__

#include "g_common.h"
#include "e_device.h"
#include "e_font.h"

/*! \addtogroup g */
/*! @{ */

/*! \name Console */
/*! @{ */

enum G_LOG_HEADERS
{
	G_LOG_NOHEADER,
	G_LOG_WARNING,
	G_LOG_ERROR,
	G_LOG_MESSAGE
};

/*!
  \brief Prints a formatted text in the log file
  \param[in] header The message header
  \param[in] format The format string (same as printf)
*/
void g_log(unsigned int header,const char* format,...);

/*! A text-mode console */
struct g_console_s
{
	char** buffer;				/*!< The console buffer */
	char* line;					/*!< The console line buffer */
	unsigned int size;			/*!< The size of the console line buffer */
	unsigned int caret[2];		/*!< The caret position */
	unsigned int width,height;	/*!< The width and height of the console */
};

typedef struct g_console_s g_console_t;

/*!
  \brief Initialises the console buffer
  \param[in] device The device to use
  \param[in] width The width of the console
  \param[in] height The height of the console
*/
void g_initconsole(e_device_t* device,unsigned int width,unsigned int height);
/*!
  \brief Prints a formatted text on the console
  \param[in] format The format string (same as printf)
*/
void g_print(const char* format,...);
/*! \brief Clears the console */
void g_clear(void);
/*!
  \brief Renders the console
  \param[in] device The device to use
  \param[in] font The font to use
*/
void g_renderconsole(e_device_t* device,e_font_t* font);
/*! \brief Destroys the console buffer */
void g_destroyconsole(void);

/*! @} */

/*! @} */

#endif

