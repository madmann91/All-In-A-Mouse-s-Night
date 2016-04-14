#include "ui_menu.h"

struct ui_menudata_s
{
	ui_menuitem_t* items;
	unsigned int numitems;
	unsigned int type;
	int* positions;
	int selected;
};

typedef struct ui_menudata_s ui_menudata_t;

static void ui_menucreate(ui_widget_t*);
static void ui_menudestroy(ui_widget_t*);
static void ui_menurender(ui_widget_t*,e_device_t*);
static void ui_menuevent(ui_widget_t*,e_event_t*);
static void* ui_menuget(ui_widget_t*,unsigned int);
static void ui_menuset(ui_widget_t*,unsigned int,void*);

const ui_widgetclass_t ui_menuclass={UI_WIDGET_MENU,ui_menucreate,ui_menudestroy,ui_menurender,ui_menuevent,ui_menuget,ui_menuset,0};

void ui_menucreate(ui_widget_t* widget)
{
	ui_menudata_t* data=(ui_menudata_t*)sys_alloc(sizeof(ui_menudata_t));
	memset(data,0,sizeof(ui_menudata_t));
	data->selected=-1;
	data->type=UI_MENU_VERTICAL;
	widget->data=data;
}

void ui_menudestroy(ui_widget_t* widget)
{
	ui_menudata_t* data=(ui_menudata_t*)widget->data;
	if(data->positions)
		sys_free(data->positions);
	sys_free(data);
}

void ui_menurender(ui_widget_t* widget,e_device_t* device)
{
	ui_menudata_t* data=(ui_menudata_t*)widget->data;
	e_renderframe(device,widget->x,widget->y,widget->width,widget->height,E_FRAME_BUMP);

	if(!widget->render||!data->items)
		return;

	if(data->type==UI_MENU_HORIZONTAL)
	{
		unsigned int i;
		int x=widget->x;
		const int margin=widget->render->font->maxheight/2;

		widget->height=2*widget->render->font->maxheight;
		for(i=0;i<data->numitems;i++)
		{
			unsigned int width,height;
			data->positions[i]=x;
			e_textsize(widget->render->font,data->items[i].text,&width,&height);
			if(i==data->selected)
				e_renderframe(device,x,widget->y,width+2*margin,widget->height-1,E_FRAME_NOBORDER);
			e_print(device,widget->render->font,x+margin,widget->y+(widget->height+widget->render->font->maxheight)/2-1,data->items[i].text);
			x+=width+2*margin;
		}
		data->positions[i]=x;
	}
	else if(data->type==UI_MENU_VERTICAL)
	{
		unsigned int i;
		int y=widget->y;
		const int margin=(7*widget->render->font->maxheight)/4;
		const int spacing=(widget->render->font->maxheight*3)/4;

		widget->height=data->numitems*margin;
		for(i=0;i<data->numitems;i++)
		{
			data->positions[i]=y;
			if(i==data->selected)
				e_renderframe(device,widget->x,y,widget->width,margin,E_FRAME_NOBORDER);
			e_print(device,widget->render->font,widget->x+spacing,y+(margin+widget->render->font->maxheight)/2-1,data->items[i].text);
			y+=margin;
		}
		data->positions[i]=y;
	}

	if(data->selected>=0)
	{
		ui_widget_t* menu=data->items[data->selected].menu;
		if(menu)
		{
			if(data->type==UI_MENU_VERTICAL)
			{
				menu->y=data->positions[data->selected];
				menu->x=widget->x+widget->width;
			}
			else if(data->type==UI_MENU_HORIZONTAL)
			{
				menu->x=data->positions[data->selected];
				menu->y=widget->y+widget->height;
			}
			ui_renderwidget(menu,device);
		}
	}
}

void ui_menuevent(ui_widget_t* widget,e_event_t* event)
{
	switch(event->type)
	{
		case E_EVENT_MOUSEMOVE:
			{
				ui_menudata_t* data=(ui_menudata_t*)widget->data;

				if(!ui_isinside(widget,event->mouse.x,event->mouse.y))
				{
					if(data->selected>=0)
					{
						ui_widget_t* menu=data->items[data->selected].menu;
						if(menu)
						{
							ui_menudata_t* menudata=(ui_menudata_t*)menu->data;

							ui_eventwidget(menu,event);
							if(menudata->selected<0)
								data->selected=-1;
						}
						else
							data->selected=-1;
					}
					break;
				}

				if(data->selected>=0)
				{
					ui_widget_t* menu=data->items[data->selected].menu;
					if(menu)
					{
						ui_menudata_t* menudata=(ui_menudata_t*)menu->data;
						menudata->selected=-1;
					}
				}

				if(data->type==UI_MENU_HORIZONTAL)
				{
					unsigned int i;
					data->selected=-1;
					if(event->mouse.x>data->positions[data->numitems])
						break;
					for(i=0;i<data->numitems;i++)
					{
						if(event->mouse.x>=data->positions[i])
							data->selected=i;
						else
							break;
					}
				}

				if(data->type==UI_MENU_VERTICAL)
				{
					unsigned int i;
					data->selected=-1;
					if(event->mouse.y>data->positions[data->numitems])
						break;
					for(i=0;i<data->numitems;i++)
					{
						if(event->mouse.y>=data->positions[i])
							data->selected=i;
						else
							break;
					}
				}
			}
			break;
		case E_EVENT_MOUSEBUTTONUP:
			if(!ui_isinside(widget,event->mouse.x,event->mouse.y))
			{
				ui_menudata_t* data=(ui_menudata_t*)widget->data;
				if(data->selected>=0)
				{
					ui_widget_t* menu=data->items[data->selected].menu;
					if(menu)
					{
						ui_menudata_t* menudata=(ui_menudata_t*)menu->data;
						ui_eventwidget(menu,event);
						if(menudata->selected<0)
							data->selected=-1;
					}
				}
			}
			else
			{
				ui_menudata_t* data=(ui_menudata_t*)widget->data;
				if(data->selected>=0)
				{
					ui_notifywidget(widget,UI_MENU_CLICKED);

					if(!data->items[data->selected].menu)
						data->selected=-1;
				}
			}
			break;
	}
}

void* ui_menuget(ui_widget_t* widget,unsigned int index)
{
	ui_menudata_t* data=(ui_menudata_t*)widget->data;
	switch(index)
	{
		case UI_MENU_SELECTION:
			return (void*)data->selected;
		case UI_MENU_TYPE:
			return (void*)data->type;
		case UI_MENU_ITEMCOUNT:
			return (void*)data->numitems;
		case UI_MENU_ITEMS:
			return (void*)data->items;
	}
	return 0;
}

void ui_menuset(ui_widget_t* widget,unsigned int index,void* param)
{
	ui_menudata_t* data=(ui_menudata_t*)widget->data;
	switch(index)
	{
		case UI_MENU_SELECTION:
			data->selected=(int)param;
			break;
		case UI_MENU_TYPE:
			data->type=(unsigned int)param;
			break;
		case UI_MENU_ITEMCOUNT:
			data->numitems=(unsigned int)param;
			data->positions=(int*)sys_realloc(data->positions,sizeof(int)*(data->numitems+1));
			memset(data->positions,0,sizeof(int)*(data->numitems+1));
			break;
		case UI_MENU_ITEMS:
			data->items=(ui_menuitem_t*)param;
			break;
	}
}
