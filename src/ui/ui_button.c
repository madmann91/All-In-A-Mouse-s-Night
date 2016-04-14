#include "ui_button.h"

struct ui_buttondata_s
{
	int pushed:1;
};

typedef struct ui_buttondata_s ui_buttondata_t;

static void ui_buttoncreate(ui_widget_t*);
static void ui_buttondestroy(ui_widget_t*);
static void ui_buttonrender(ui_widget_t*,e_device_t*);
static void ui_buttonevent(ui_widget_t*,e_event_t*);
static void* ui_buttonget(ui_widget_t*,unsigned int);
static void ui_buttonset(ui_widget_t*,unsigned int,void*);

const ui_widgetclass_t ui_buttonclass={UI_WIDGET_BUTTON,ui_buttoncreate,ui_buttondestroy,ui_buttonrender,ui_buttonevent,ui_buttonget,ui_buttonset,0};

void ui_buttoncreate(ui_widget_t* widget)
{
	ui_buttondata_t* data=(ui_buttondata_t*)sys_alloc(sizeof(ui_buttondata_t));
	data->pushed=0;
	widget->data=data;
}

void ui_buttondestroy(ui_widget_t* widget)
{
	sys_free(widget->data);
}

void ui_buttonrender(ui_widget_t* widget,e_device_t* device)
{
	ui_buttondata_t* data=(ui_buttondata_t*)widget->data;

	e_renderframe(device,widget->x,widget->y,widget->width,widget->height,(data->pushed)?0:E_FRAME_SUNKEN|E_FRAME_BUMP);
	if(widget->caption&&widget->render)
	{
		unsigned int width,height;
		e_textsize(widget->render->font,widget->caption,&width,&height);
		e_print(device,widget->render->font,widget->x+(widget->width-width)/2,widget->y+(widget->height+height)/2,widget->caption);
	}
}

void ui_buttonevent(ui_widget_t* widget,e_event_t* event)
{
	switch(event->type)
	{
		case E_EVENT_MOUSEBUTTONDOWN:
			if(ui_isinside(widget,event->mouse.x,event->mouse.y))
			{
				ui_buttondata_t* data=(ui_buttondata_t*)widget->data;
				data->pushed=1;
			}
			break;
		case E_EVENT_MOUSEBUTTONUP:
			{
				ui_buttondata_t* data=(ui_buttondata_t*)widget->data;
				if(data->pushed&&ui_isinside(widget,event->mouse.x,event->mouse.y))
					ui_notifywidget(widget,UI_BUTTON_CLICKED);
				data->pushed=0;
			}
			break;
	}
}

void* ui_buttonget(ui_widget_t* widget,unsigned int index)
{
	if(index==UI_BUTTON_STATE)
	{
		ui_buttondata_t* data=(ui_buttondata_t*)widget->data;
		return (void*)data->pushed;
	}
	return 0;
}

void ui_buttonset(ui_widget_t* widget,unsigned int index,void* param)
{
	if(index==UI_BUTTON_STATE)
	{
		ui_buttondata_t* data=(ui_buttondata_t*)widget->data;
		data->pushed=(int)param;
	}
}
