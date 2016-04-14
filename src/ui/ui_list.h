/***********************************************************
*                                                          *
*  File : ui_list.h                                        *
*  Date : 13/11/2009                                       *
*  Comments : List widget                                  *
*                                                          *
************************************************************/

#ifndef __UI_LIST_H__
#define __UI_LIST_H__

#include "ui_common.h"

/*! \addtogroup ui */
/*! @{ */

/*! \name List */
/*! @{ */

struct ui_listcolumn_s
{
	char* title;
	char** items;
	unsigned int numitems;
	unsigned int width;
};

typedef struct ui_listcolumn_s ui_listcolumn_t;

enum UI_LIST_PROPERTIES
{
	UI_LIST_SELECTION,
	UI_LIST_COLUMNCOUNT,
	UI_LIST_COLUMNS
};

extern const ui_widgetclass_t ui_listclass;

/*! @} */

/*! @} */

#endif
