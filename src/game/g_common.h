/***********************************************************
*                                                          *
*  File : g_common.h                                       *
*  Date : 13/11/2009                                       *
*  Comments : Game module definitions                      *
*                                                          *
************************************************************/

#ifndef __G_COMMON_H__
#define __G_COMMON_H__

/*! \addtogroup g Game module */
/*! @{ */

/*! \brief Initializes the game module */
int g_init(void);
/*! \brief Runs the game module */
void g_run(void);
/*! \brief Destroys the memory used by the game module */
void g_quit(void);

/*! @} */

#endif
