/***********************************************************
*                                                          *
*  File : g_interface.h                                    *
*  Date : 13/11/2009                                       *
*  Comments : The game user interface                      *
*                                                          *
************************************************************/

#ifndef __G_INTERFACE_H__
#define __G_INTERFACE_H__

#include "g_common.h"
#include "e_device.h"

/*! \addtogroup g */
/*! @{ */

/*! \name Interface */
/*! @{ */

/*!
  \brief Initializes the game user interface
  \param[in] device The device to use
  \return 0 if an error occured, otherwise 1
*/
int g_initinterface(e_device_t* device);
/*!
  \brief Renders the user interface
  \param[in] device The device to use
*/
void g_renderinterface(e_device_t* device);
/*!
  \brief Handles user interface events
  \param[in] event The device event to handle
*/
int g_eventinterface(e_event_t* event);
/*! \brief Destroys the user interface objects */
void g_destroyinterface(void);

/*! @} */

/*! @} */

#endif
