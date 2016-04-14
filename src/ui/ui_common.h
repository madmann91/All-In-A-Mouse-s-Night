/***********************************************************
*                                                          *
*  File : ui_common.h                                      *
*  Date : 13/11/2009                                       *
*  Comments : User interface definitions                   *
*                                                          *
************************************************************/

#ifndef __UI_COMMON_H__
#define __UI_COMMON_H__

#include "sys_common.h"
#include "e_device.h"
#include "e_font.h"

/*! \addtogroup ui User Interface module */
/*! @{ */

enum UI_WIDGET_TYPE
{
	UI_WIDGET_BUTTON,
	UI_WIDGET_TEXT,
	UI_WIDGET_EDIT,
	UI_WIDGET_CHECK,
	UI_WIDGET_LIST,
	UI_WIDGET_PROGRESS,
	UI_WIDGET_TAB,
	UI_WIDGET_MENU,
	UI_WIDGET_FRAME
};

enum UI_WIDGET_FLAGS
{
	UI_FLAG_VISIBLE=0x01,
	UI_FLAG_ENABLED=0x02
};

struct ui_widget_s;
typedef struct ui_widget_s ui_widget_t;

typedef void (*ui_createfunc_t)(ui_widget_t*);
typedef void (*ui_destroyfunc_t)(ui_widget_t*);
typedef void (*ui_renderfunc_t)(ui_widget_t*,e_device_t*);
typedef void (*ui_eventfunc_t)(ui_widget_t*,e_event_t*);
typedef void* (*ui_getfunc_t)(ui_widget_t*,unsigned int);
typedef void (*ui_setfunc_t)(ui_widget_t*,unsigned int,void*);
typedef void (*ui_notifyfunc_t)(ui_widget_t*,unsigned int);

struct ui_widgetrender_s
{
	e_font_t* font;
};

typedef struct ui_widgetrender_s ui_widgetrender_t;

struct ui_widgetclass_s
{
	unsigned int type;					/*!< The widget type */

	ui_createfunc_t createfunc;			/*!< Creation callback */
	ui_destroyfunc_t destroyfunc;		/*!< Destruction callback */
	ui_renderfunc_t renderfunc;			/*!< Rendering callback */
	ui_eventfunc_t eventfunc;			/*!< Event handling callback */
	ui_getfunc_t getfunc;				/*!< Getter callback */
	ui_setfunc_t setfunc;				/*!< Setter callback */
	ui_notifyfunc_t notifyfunc;			/*!< Notification callback (user value) */
};

typedef struct ui_widgetclass_s ui_widgetclass_t;

struct ui_widget_s
{
	ui_widgetclass_t type;			/*!< The widget class */
	void* user;						/*!< The widget user data (user-chosen value) */
	int flags;						/*!< The widget flags */

	char* caption;					/*!< The widget caption */

	unsigned int width;				/*!< Widget width */
	unsigned int height;			/*!< Widget height */
	int x;							/*!< Horizontal position */
	int y;							/*!< Vertical position */

	ui_widgetrender_t* render;		/*!< Rendering info */

	void* data;						/*!< Private data */
};

/*!
  \brief Creates a widget
  \param[in] classid Widget class
  \param[in] x Horizontal position
  \param[in] y Vertical position
  \param[in] width Widget width
  \param[in] height Widget height
  \return The newly created widget
*/
ui_widget_t* ui_createwidget(const ui_widgetclass_t* classid,int x,int y,unsigned int width,unsigned int height);
/*!
  \brief Destroys a widget
  \param[in] widget The widget to destroy
*/
void ui_destroywidget(ui_widget_t* widget);
/*!
  \brief Renders a widget
  \param[in] widget The widget to render
  \param[in] device The device to render in
*/
void ui_renderwidget(ui_widget_t* widget,e_device_t* device);
/*!
  \brief Sends an event to a widget
  \param[in] widget The widget send an event to
  \param[in] event The event
*/
void ui_eventwidget(ui_widget_t* widget,e_event_t* event);
/*!
  \brief Gets a widget property
  \param[in] widget The widget to get the property from
  \param[in] property The property, depends on the widget class
  \return The property value
*/
void* ui_widgetget(ui_widget_t* widget,unsigned int property);
/*!
  \brief Sets a widget property
  \param[in] widget The widget
  \param[in] property The property, depends on the widget class
  \param[in] data The property value
*/
void ui_widgetset(ui_widget_t* widget,unsigned int property,void* data);
/*!
  \brief Calls the notication function for the widget
  \param[in] widget The widget to notify
  \param[in] notify The notification code
*/
void ui_notifywidget(ui_widget_t* widget,unsigned int notify);
/*!
  \brief Tests if a point is inside a widget
  \param[in] widget The widget to test
  \param[in] x Horizontal position of the point
  \param[in] y Vertical position of the point
  \return 1 if the point is inside the widget
*/
int ui_isinside(ui_widget_t* widget,int x,int y);

/*! @} */

#endif
