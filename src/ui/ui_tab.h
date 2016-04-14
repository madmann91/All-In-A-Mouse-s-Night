/***********************************************************
*                                                          *
*  File : ui_tab.h                                         *
*  Date : 13/11/2009                                       *
*  Comments : Tab widget                                   *
*                                                          *
************************************************************/

#ifndef __UI_TAB_H__
#define __UI_TAB_H__

#include "ui_common.h"

/*! \addtogroup ui */
/*! @{ */

/*! \name Tab */
/*! @{ */

struct ui_tabelement_s
{
	ui_widget_t** widgets;
	unsigned int numwidgets;
	char* text;
};

typedef struct ui_tabelement_s ui_tabelement_t;

enum UI_TAB_PROPERTIES
{
	UI_TAB_SELECTION,
	UI_TAB_ELEMENTCOUNT,
	UI_TAB_ELEMENTS
};

extern const ui_widgetclass_t ui_tabclass;

/*! @} */

/*! @} */

#endif
