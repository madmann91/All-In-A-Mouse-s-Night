/***********************************************************
*                                                          *
*  File : ui_menu.h                                        *
*  Date : 13/11/2009                                       *
*  Comments : Menu widget                                  *
*                                                          *
************************************************************/

#ifndef __UI_MENU_H__
#define __UI_MENU_H__

#include "ui_common.h"

/*! \addtogroup ui */
/*! @{ */

/*! \name Menu */
/*! @{ */

struct ui_menuitem_s
{
	char* text;
	ui_widget_t* menu;
};

typedef struct ui_menuitem_s ui_menuitem_t;

enum UI_MENU_TYPES
{
	UI_MENU_VERTICAL,
	UI_MENU_HORIZONTAL
};

enum UI_MENU_PROPERTIES
{
	UI_MENU_TYPE,
	UI_MENU_SELECTION,
	UI_MENU_ITEMCOUNT,
	UI_MENU_ITEMS
};

enum UI_MENU_NOTIFICATIONS
{
	UI_MENU_CLICKED
};

extern const ui_widgetclass_t ui_menuclass;

/*! @} */

/*! @} */

#endif
