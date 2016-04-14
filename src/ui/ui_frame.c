#include "ui_frame.h"

struct ui_framedata_s
{
	ui_widget_t** widgets;
	unsigned int numwidgets;
	unsigned int height;
	int moving;
	int x;
	int y;
};

typedef struct ui_framedata_s ui_framedata_t;

static void ui_framecreate(ui_widget_t*);
static void ui_framedestroy(ui_widget_t*);
static void ui_framerender(ui_widget_t*,e_device_t*);
static void ui_frameevent(ui_widget_t*,e_event_t*);
static void* ui_frameget(ui_widget_t*,unsigned int);
static void ui_frameset(ui_widget_t*,unsigned int,void*);

const ui_widgetclass_t ui_frameclass={UI_WIDGET_FRAME,ui_framecreate,ui_framedestroy,ui_framerender,ui_frameevent,ui_frameget,ui_frameset,0};

void ui_framecreate(ui_widget_t* widget)
{
	ui_framedata_t* data=(ui_framedata_t*)sys_alloc(sizeof(ui_framedata_t));
	memset(data,0,sizeof(ui_framedata_t));
	widget->data=data;
}

void ui_framedestroy(ui_widget_t* widget)
{
	sys_free(widget->data);
}

void ui_framerender(ui_widget_t* frame,e_device_t* device)
{
	ui_framedata_t* data=(ui_framedata_t*)frame->data;
	unsigned int i;

	e_renderframe(device,frame->x,frame->y,frame->width,frame->height,E_FRAME_BUMP);

	if(frame->render)
	{
		data->height=5*frame->render->font->maxheight/3;
		e_renderframe(device,frame->x+1,frame->y,frame->width-1,data->height,E_FRAME_NOBORDER|E_FRAME_SUNKEN);
		e_print(device,frame->render->font,frame->x+frame->render->font->maxheight/2,frame->y+(data->height+frame->render->font->maxheight)/2-1,frame->caption);
	}

	for(i=0;i<data->numwidgets;i++)
	{
		ui_widget_t* widget=data->widgets[i];
		ui_renderwidget(widget,device);
	}
}

void ui_frameevent(ui_widget_t* frame,e_event_t* event)
{
	ui_framedata_t* data=(ui_framedata_t*)frame->data;
	switch(event->type)
	{
		case E_EVENT_MOUSEBUTTONDOWN:
			if(event->mouse.x>=frame->x&&event->mouse.x<=frame->x+(int)frame->width&&event->mouse.y>=frame->y)
			{
				if(event->mouse.y<=frame->y+(int)data->height)
				{
					data->x=event->mouse.x;
					data->y=event->mouse.y;
					data->moving=1;
				}
				else
				{
					if(event->mouse.y<=frame->y+(int)frame->height)
					{
						unsigned int i;
						for(i=0;i<data->numwidgets;i++)
						{
							ui_widget_t* widget=data->widgets[i];
							ui_eventwidget(widget,event);
						}
					}
				}
			}
			return;
		case E_EVENT_MOUSEBUTTONUP:
			data->moving=0;
			if(ui_isinside(frame,event->mouse.x,event->mouse.y))
			{
				unsigned int i;
				for(i=0;i<data->numwidgets;i++)
				{
					ui_widget_t* widget=data->widgets[i];
					ui_eventwidget(widget,event);
				}
			}
			break;
		case E_EVENT_MOUSEMOVE:
			if(data->moving)
			{
				unsigned int i;
				frame->x+=event->mouse.x-data->x;
				frame->y+=event->mouse.y-data->y;
				for(i=0;i<data->numwidgets;i++)
				{
					ui_widget_t* widget=data->widgets[i];
					widget->x+=event->mouse.x-data->x;
					widget->y+=event->mouse.y-data->y;
				}
				data->x=event->mouse.x;
				data->y=event->mouse.y;
			}
			else
			{
				if(ui_isinside(frame,event->mouse.x,event->mouse.y))
				{
					unsigned int i;
					for(i=0;i<data->numwidgets;i++)
					{
						ui_widget_t* widget=data->widgets[i];
						ui_eventwidget(widget,event);
					}
				}
			}
			break;
		default:
			{
				unsigned int i;
				for(i=0;i<data->numwidgets;i++)
				{
					ui_widget_t* widget=data->widgets[i];
					ui_eventwidget(widget,event);
				}
			}
			break;
	}
}

void* ui_frameget(ui_widget_t* widget,unsigned int index)
{
	ui_framedata_t* data=(ui_framedata_t*)widget->data;
	switch(index)
	{
		case UI_FRAME_WIDGETCOUNT:
			return (void*)data->numwidgets;
		case UI_FRAME_WIDGETS:
			return (void*)data->widgets;
	}
	return 0;
}

void ui_frameset(ui_widget_t* widget,unsigned int index,void* param)
{
	ui_framedata_t* data=(ui_framedata_t*)widget->data;
	switch(index)
	{
		case UI_FRAME_WIDGETCOUNT:
			data->numwidgets=(unsigned int)param;
			break;
		case UI_FRAME_WIDGETS:
			data->widgets=(ui_widget_t**)param;
			break;
	}
}
