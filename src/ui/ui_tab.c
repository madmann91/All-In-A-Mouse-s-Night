#include "ui_tab.h"

struct ui_tabdata_s
{
	ui_tabelement_t* elements;
	unsigned int numelements;
	unsigned int height;
	int* positions;
	int selected;
};

typedef struct ui_tabdata_s ui_tabdata_t;

static void ui_tabcreate(ui_widget_t*);
static void ui_tabdestroy(ui_widget_t*);
static void ui_tabrender(ui_widget_t*,e_device_t*);
static void ui_tabevent(ui_widget_t*,e_event_t*);
static void* ui_tabget(ui_widget_t*,unsigned int);
static void ui_tabset(ui_widget_t*,unsigned int,void*);

const ui_widgetclass_t ui_tabclass={UI_WIDGET_TAB,ui_tabcreate,ui_tabdestroy,ui_tabrender,ui_tabevent,ui_tabget,ui_tabset,0};

void ui_tabcreate(ui_widget_t* widget)
{
	ui_tabdata_t* data=(ui_tabdata_t*)sys_alloc(sizeof(ui_tabdata_t));
	memset(data,0,sizeof(ui_tabdata_t));
	widget->data=data;
}

void ui_tabdestroy(ui_widget_t* widget)
{
	ui_tabdata_t* data=(ui_tabdata_t*)widget->data;
	if(data->positions)
		sys_free(data->positions);
	sys_free(data);
}

void ui_tabrender(ui_widget_t* widget,e_device_t* device)
{
	ui_tabdata_t* data=(ui_tabdata_t*)widget->data;
	unsigned int i;
	unsigned int pos;
	unsigned int selwidth=0;
	
	if(!widget->render->font)
	{
		e_renderframe(device,widget->x,widget->y,widget->width,widget->height,E_FRAME_SUNKEN|E_FRAME_BUMP);	
		return;
	}
	data->height=widget->render->font->maxheight*2;
	e_renderframe(device,widget->x,widget->y+data->height,widget->width,widget->height-data->height,E_FRAME_NOTOP|E_FRAME_BUMP);
	pos=widget->x;
	for(i=0;i<data->numelements;i++)
	{
		unsigned int height,width;
		e_textsize(widget->render->font,data->elements[i].text,&width,&height);
		data->positions[i]=pos;
		width+=data->height;
		if(i!=data->selected)
		{
			e_renderframe(device,pos,widget->y+2,width,data->height-2,E_FRAME_BUMP|E_FRAME_NOBOTTOM|E_FRAME_SUNKEN);
			e_print(device,widget->render->font,pos+widget->render->font->maxheight,widget->y+(widget->render->font->maxheight+data->height)/2,data->elements[i].text);		
		}
		else
			selwidth=width;
		pos+=width+1;
	}
	if(data->positions)
		data->positions[i]=pos;
	if(data->selected>=0&&data->selected<(int)data->numelements)
	{
		e_renderframe(device,data->positions[data->selected],widget->y,selwidth,data->height,E_FRAME_BUMP|E_FRAME_NOBOTTOM);
		e_print(device,widget->render->font,data->positions[data->selected]+widget->render->font->maxheight,widget->y+(widget->render->font->maxheight+data->height)/2,data->elements[data->selected].text);
	}
}

void ui_tabevent(ui_widget_t* widget,e_event_t* event)
{
	switch(event->type)
	{
		case E_EVENT_MOUSEBUTTONDOWN:
			if(event->mouse.x>=widget->x&&event->mouse.x<=widget->x+(int)widget->width&&event->mouse.y>=widget->y)
			{
				ui_tabdata_t* data=(ui_tabdata_t*)widget->data;
				if(!data->positions)
					break;
				if(event->mouse.y<=widget->y+(int)data->height)
				{
					unsigned int i;
					unsigned int selection=data->selected;

					if(event->mouse.x>=data->positions[data->numelements])
						return;

					for(i=0;i<data->numelements;i++)
					{
						if(event->mouse.x>=data->positions[i])
							selection=i;
					}

					if(selection!=data->selected)
					{
						unsigned int j;

						data->selected=selection;
						for(i=0;i<data->numelements;i++)
						{
							for(j=0;j<data->elements[i].numwidgets;j++)
							{
								if(i==data->selected)
									data->elements[i].widgets[j]->flags|=(UI_FLAG_VISIBLE|UI_FLAG_ENABLED);
								else
									data->elements[i].widgets[j]->flags&=(~UI_FLAG_VISIBLE)&(~UI_FLAG_ENABLED);
							}
						}
					}
				}
			}
			break;
	}
}

void* ui_tabget(ui_widget_t* widget,unsigned int index)
{
	ui_tabdata_t* data=(ui_tabdata_t*)widget->data;
	switch(index)
	{
		case UI_TAB_SELECTION:
			return (void*)data->selected;
		case UI_TAB_ELEMENTCOUNT:
			return (void*)data->numelements;
		case UI_TAB_ELEMENTS:
			return (void*)data->elements;
	}
	return 0;
}

void ui_tabset(ui_widget_t* widget,unsigned int index,void* param)
{
	ui_tabdata_t* data=(ui_tabdata_t*)widget->data;
	switch(index)
	{
		case UI_TAB_SELECTION:
			data->selected=(int)param;
			break;
		case UI_TAB_ELEMENTCOUNT:
			data->numelements=(unsigned int)param;
			data->positions=(int*)sys_realloc(data->positions,sizeof(int)*(data->numelements+1));
			memset(data->positions,0,sizeof(int)*(data->numelements+1));
			break;
		case UI_TAB_ELEMENTS:
			data->elements=(ui_tabelement_t*)param;
			break;
	}
}