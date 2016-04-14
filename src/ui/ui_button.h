/***********************************************************
*                                                          *
*  File : ui_button.h                                      *
*  Date : 13/11/2009                                       *
*  Comments : Button widget                                *
*                                                          *
************************************************************/

#ifndef __UI_BUTTON_H__
#define __UI_BUTTON_H__

#include "ui_common.h"

/*! \addtogroup ui */
/*! @{ */

/*! \name Button */
/*! @{ */

enum UI_BUTTON_PROPERTIES
{
	UI_BUTTON_STATE
};

enum UI_BUTTON_NOTIFICATIONS
{
	UI_BUTTON_CLICKED
};

extern const ui_widgetclass_t ui_buttonclass;

/*! @} */

/*! @} */

#endif
