#include "ui_check.h"

struct ui_checkdata_s
{
	int pushed:1;
};

typedef struct ui_checkdata_s ui_checkdata_t;

static void ui_checkcreate(ui_widget_t*);
static void ui_checkdestroy(ui_widget_t*);
static void ui_checkrender(ui_widget_t*,e_device_t*);
static void ui_checkevent(ui_widget_t*,e_event_t*);
static void* ui_checkget(ui_widget_t*,unsigned int);
static void ui_checkset(ui_widget_t*,unsigned int,void*);

const ui_widgetclass_t ui_checkclass={UI_WIDGET_CHECK,ui_checkcreate,ui_checkdestroy,ui_checkrender,ui_checkevent,ui_checkget,ui_checkset,0};

void ui_checkcreate(ui_widget_t* widget)
{
	ui_checkdata_t* data=(ui_checkdata_t*)sys_alloc(sizeof(ui_checkdata_t));
	data->pushed=0;
	widget->data=data;
}

void ui_checkdestroy(ui_widget_t* widget)
{
	sys_free(widget->data);
}

void ui_checkrender(ui_widget_t* widget,e_device_t* device)
{
	ui_checkdata_t* data=(ui_checkdata_t*)widget->data;

	e_renderframe(device,widget->x,widget->y,widget->width,widget->height,E_FRAME_SUNKEN);
	if(data->pushed)
		e_renderframe(device,widget->x+1,widget->y+1,widget->width-3,widget->height-3,E_FRAME_NOBORDER);
}

void ui_checkevent(ui_widget_t* widget,e_event_t* event)
{
	switch(event->type)
	{
		case E_EVENT_MOUSEBUTTONDOWN:
			if(ui_isinside(widget,event->mouse.x,event->mouse.y))
			{
				ui_checkdata_t* data=(ui_checkdata_t*)widget->data;
				data->pushed=!data->pushed;
			}
			break;
	}
}

void* ui_checkget(ui_widget_t* widget,unsigned int index)
{
	if(index==UI_CHECK_STATE)
	{
		ui_checkdata_t* data=(ui_checkdata_t*)widget->data;
		return (void*)data->pushed;
	}
	return 0;
}

void ui_checkset(ui_widget_t* widget,unsigned int index,void* param)
{
	if(index==UI_CHECK_STATE)
	{
		ui_checkdata_t* data=(ui_checkdata_t*)widget->data;
		data->pushed=(int)param;
	}
}
