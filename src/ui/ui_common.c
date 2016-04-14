#include "ui_common.h"

ui_widget_t* ui_createwidget(const ui_widgetclass_t* type,int x,int y,unsigned int width,unsigned int height)
{
	ui_widget_t* widget=(ui_widget_t*)sys_alloc(sizeof(ui_widget_t));
	memset(widget,0,sizeof(ui_widget_t));

	widget->x=x;
	widget->y=y;
	widget->width=width;
	widget->height=height;
	widget->flags=UI_FLAG_VISIBLE|UI_FLAG_ENABLED;

	widget->type=*type;
	type->createfunc(widget);

	return widget;
}

void ui_destroywidget(ui_widget_t* widget)
{
	widget->type.destroyfunc(widget);
	sys_free(widget);
}

void ui_renderwidget(ui_widget_t* widget,e_device_t* device)
{
	if(widget->flags&UI_FLAG_VISIBLE)
		widget->type.renderfunc(widget,device);
}

void ui_eventwidget(ui_widget_t* widget,e_event_t* event)
{
	if(widget->flags&UI_FLAG_ENABLED)
		widget->type.eventfunc(widget,event);
}

void* ui_widgetget(ui_widget_t* widget,unsigned int index)
{
	return widget->type.getfunc(widget,index);
}

void ui_widgetset(ui_widget_t* widget,unsigned int index,void* data)
{
	widget->type.setfunc(widget,index,data);
}

void ui_notifywidget(ui_widget_t* widget,unsigned int code)
{
	if(widget->type.notifyfunc)
		widget->type.notifyfunc(widget,code);
}

int ui_isinside(ui_widget_t* widget,int x,int y)
{
	if(x>=widget->x&&y>=widget->y&&
		x<=widget->x+(int)widget->width&&
		y<=widget->y+(int)widget->height)
		return 1;

	return 0;
}
