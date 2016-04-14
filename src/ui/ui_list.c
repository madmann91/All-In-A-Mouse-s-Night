#include "ui_list.h"

struct ui_listdata_s
{
	ui_listcolumn_t* columns;
	unsigned int numcolumns;

	unsigned int step;
	unsigned int height;
	unsigned int maxitems;

	int selected;
};

typedef struct ui_listdata_s ui_listdata_t;

static void ui_listcreate(ui_widget_t*);
static void ui_listdestroy(ui_widget_t*);
static void ui_listrender(ui_widget_t*,e_device_t*);
static void ui_listevent(ui_widget_t*,e_event_t*);
static void* ui_listget(ui_widget_t*,unsigned int);
static void ui_listset(ui_widget_t*,unsigned int,void*);

const ui_widgetclass_t ui_listclass={UI_WIDGET_LIST,ui_listcreate,ui_listdestroy,ui_listrender,ui_listevent,ui_listget,ui_listset,0};

void ui_listcreate(ui_widget_t* widget)
{
	ui_listdata_t* data=(ui_listdata_t*)sys_alloc(sizeof(ui_listdata_t));
	data->numcolumns=0;
	data->columns=0;

	data->step=1;
	data->height=0;
	data->maxitems=0;

	data->selected=-1;
	widget->data=data;
}

void ui_listdestroy(ui_widget_t* widget)
{
	sys_free(widget->data);
}

void ui_listrender(ui_widget_t* widget,e_device_t* device)
{
	ui_listdata_t* data=(ui_listdata_t*)widget->data;
	unsigned int i;
	unsigned int x=widget->x+1;

	e_renderframe(device,widget->x,widget->y,widget->width,widget->height,E_FRAME_SUNKEN);
	if(!widget->render)
		return;

	data->step=widget->render->font->maxheight+2;
	data->height=(3*widget->render->font->maxheight)/2;
	data->maxitems=0;

	if(data->selected>=0)
		e_renderframe(device,widget->x+1,widget->y+data->height+data->step*(data->selected)+1,widget->width-1,data->step,E_FRAME_NOBORDER);

	for(i=0;i<data->numcolumns;i++)
	{
		unsigned int y=widget->y+data->height+data->step-2,j;

		e_renderframe(device,x,widget->y+1,data->columns[i].width,data->height,E_FRAME_BUMP|E_FRAME_SUNKEN);
		e_print(device,widget->render->font,x+widget->render->font->maxheight/2,widget->y+(data->height+widget->render->font->maxheight)/2,data->columns[i].title);

		if(data->maxitems<data->columns[i].numitems)
			data->maxitems=data->columns[i].numitems;

		for(j=0;j<data->columns[i].numitems;j++)
		{
			e_print(device,widget->render->font,x,y,data->columns[i].items[j]);
			y+=data->step;
		}
		x+=data->columns[i].width+1;
	}
}

void ui_listevent(ui_widget_t* widget,e_event_t* event)
{
	switch(event->type)
	{
		case E_EVENT_MOUSEBUTTONDOWN:
			if(ui_isinside(widget,event->mouse.x,event->mouse.y))
			{
				ui_listdata_t* data=(ui_listdata_t*)widget->data;
				if(event->mouse.y>widget->y+(int)data->height)
				{
					data->selected=(event->mouse.y-widget->y-data->height-1)/data->step;
					if(data->selected>=(int)data->maxitems)
						data->selected=-1;
				}
			}
			break;
	}
}

void* ui_listget(ui_widget_t* widget,unsigned int index)
{
	ui_listdata_t* data=(ui_listdata_t*)widget->data;
	switch(index)
	{
		case UI_LIST_SELECTION:
			return (void*)data->selected;
		case UI_LIST_COLUMNCOUNT:
			return (void*)data->numcolumns;
		case UI_LIST_COLUMNS:
			return (void*)data->columns;
	}
	return 0;
}

void ui_listset(ui_widget_t* widget,unsigned int index,void* param)
{
	ui_listdata_t* data=(ui_listdata_t*)widget->data;
	switch(index)
	{
		case UI_LIST_SELECTION:
			data->selected=(int)param;
			break;
		case UI_LIST_COLUMNCOUNT:
			data->numcolumns=(unsigned int)param;
			break;
		case UI_LIST_COLUMNS:
			data->columns=(ui_listcolumn_t*)param;
			break;
	}
}
