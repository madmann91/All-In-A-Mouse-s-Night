#include "ui_progress.h"

#define UI_PROGRESS_SIZE 10
#define UI_PROGRESS_GAP 2

struct ui_progressdata_s
{
	unsigned int progress;
	unsigned int max;
};

typedef struct ui_progressdata_s ui_progressdata_t;

static void ui_progresscreate(ui_widget_t*);
static void ui_progressdestroy(ui_widget_t*);
static void ui_progressrender(ui_widget_t*,e_device_t*);
static void ui_progressevent(ui_widget_t*,e_event_t*);
static void* ui_progressget(ui_widget_t*,unsigned int);
static void ui_progressset(ui_widget_t*,unsigned int,void*);

const ui_widgetclass_t ui_progressclass={UI_WIDGET_PROGRESS,ui_progresscreate,ui_progressdestroy,ui_progressrender,ui_progressevent,ui_progressget,ui_progressset,0};

void ui_progresscreate(ui_widget_t* widget)
{
	ui_progressdata_t* data=(ui_progressdata_t*)sys_alloc(sizeof(ui_progressdata_t));
	data->progress=0;
	data->max=100;
	widget->data=data;
}

void ui_progressdestroy(ui_widget_t* widget)
{
	sys_free(widget->data);
}

void ui_progressrender(ui_widget_t* widget,e_device_t* device)
{
	ui_progressdata_t* data=(ui_progressdata_t*)widget->data;
	const unsigned int width=((widget->width-1)*data->progress)/data->max;

	e_renderframe(device,widget->x,widget->y,widget->width,widget->height,E_FRAME_SUNKEN);

	if(widget->flags&UI_FLAG_ENABLED)
		e_renderframe(device,widget->x+1,widget->y+1,width,widget->height-2,E_FRAME_NOBORDER);
	else
	{
		const unsigned int num=width/(UI_PROGRESS_SIZE+UI_PROGRESS_GAP);
		unsigned int i;
		unsigned int x=widget->x+1;

		for(i=0;i<num;i++)
		{
			e_renderframe(device,x,widget->y+1,UI_PROGRESS_SIZE,widget->height-3,E_FRAME_NOBORDER);
			x+=UI_PROGRESS_SIZE+UI_PROGRESS_GAP;
		}
	}
}

void ui_progressevent(ui_widget_t* widget,e_event_t* event)
{
	switch(event->type)
	{
		case E_EVENT_MOUSEBUTTONDOWN:
			if(ui_isinside(widget,event->mouse.x,event->mouse.y))
			{
				ui_progressdata_t* data=(ui_progressdata_t*)widget->data;
				data->progress=((event->mouse.x-widget->x)*data->max+widget->width/2)/widget->width;
			}
			break;
	}
}

void* ui_progressget(ui_widget_t* widget,unsigned int index)
{
	ui_progressdata_t* data=(ui_progressdata_t*)widget->data;
	switch(index)
	{
		case UI_PROGRESS_POSITION:
			return (void*)data->progress;
		case UI_PROGRESS_MAXIMUM:
			return (void*)data->max;
	}
	return 0;
}

void ui_progressset(ui_widget_t* widget,unsigned int index,void* param)
{
	ui_progressdata_t* data=(ui_progressdata_t*)widget->data;
	switch(index)
	{
		case UI_PROGRESS_POSITION:
			data->progress=(unsigned int)param;
			break;
		case UI_PROGRESS_MAXIMUM:
			data->max=(unsigned int)param;
			break;
	}
}
