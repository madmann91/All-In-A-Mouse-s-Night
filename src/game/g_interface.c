#include "config.h"
#include "g_variables.h"
#include "g_interface.h"
#include "g_console.h"

#include "e_camera.h"

#include "ui_button.h"
#include "ui_check.h"
#include "ui_edit.h"
#include "ui_frame.h"
#include "ui_list.h"
#include "ui_menu.h"
#include "ui_progress.h"
#include "ui_tab.h"
#include "ui_text.h"

#include "g_game.h"

#include "fs_common.h"

#define G_INTERFACE_FONTSIZE 10

struct g_frame_s
{
	ui_widget_t* frame;
	ui_widget_t** widgets;
	unsigned int numwidgets;
};

typedef struct g_frame_s g_frame_t;

struct g_menu_s
{
	ui_widget_t* menu;
	ui_menuitem_t* items;
	unsigned int numitems;
};

typedef struct g_menu_s g_menu_t;

struct g_interface_s
{
	e_device_t* device;

	g_frame_t* frames;
	unsigned int numframes;

	g_menu_t* menus;
	unsigned int nummenus;

	ui_widgetrender_t render;
	int focus;
	int status;
};

typedef struct g_interface_s g_interface_t;

g_interface_t g_interface;

static void g_centerframe(g_frame_t* frame)
{
	int x=(g_interface.device->width-frame->frame->width)/2-frame->frame->x;
	int y=(g_interface.device->height-frame->frame->height)/2-frame->frame->y;
	unsigned int i;

	//Center the frame on screen
	frame->frame->x+=x;
	frame->frame->y+=y;
	for(i=0;i<frame->numwidgets;i++)
	{
		frame->widgets[i]->x+=x;
		frame->widgets[i]->y+=y;
	}
}

static void g_destroywidget(ui_widget_t* widget)
{
	switch(widget->type.type)
	{
		case UI_WIDGET_LIST:
			{
				unsigned int i,j,count;
				ui_listcolumn_t* columns;

				count=(unsigned int)ui_widgetget(widget,UI_LIST_COLUMNCOUNT);
				columns=(ui_listcolumn_t*)ui_widgetget(widget,UI_LIST_COLUMNS);

				//Destroy column
				for(i=0;i<count;i++)
				{
					if(columns[i].numitems)
					{
						for(j=0;j<columns[i].numitems;j++)
							sys_free(columns[i].items[j]);
						sys_free(columns[i].items);
					}
				}
				sys_free(columns);
			}
			break;
		case UI_WIDGET_EDIT:
			sys_free(widget->caption);
			break;
	}
}

static void g_notifygamemenu(ui_widget_t* menu,unsigned int code)
{
	int sel;

	if(code!=UI_MENU_CLICKED)
		return;

	//Get the selection
	sel=(int)ui_widgetget(menu,UI_MENU_SELECTION);
	if(sel<0)
		return;
	switch(sel)
	{
		case 0:
			//Show new game frame
			if(!(g_interface.frames[0].frame->flags&UI_FLAG_VISIBLE))
			{
				g_centerframe(&g_interface.frames[0]);
				g_interface.frames[0].frame->flags|=UI_FLAG_VISIBLE;
				g_interface.focus=0;
			}
			break;
		case 2:
			g_interface.status=0;
			break;
	}
}

static void g_notifyhelpmenu(ui_widget_t* menu,unsigned int code)
{
	if(code!=UI_MENU_CLICKED||(int)ui_widgetget(menu,UI_MENU_SELECTION)<0)
		return;

	if(!(g_interface.frames[1].frame->flags&UI_FLAG_VISIBLE))
	{
		g_centerframe(&g_interface.frames[1]);
		g_interface.frames[1].frame->flags|=UI_FLAG_VISIBLE;
		g_interface.focus=1;
	}
}

static void g_notifygameframe(ui_widget_t* widget,unsigned int code)
{
	if(widget->user)
	{
		//Retreive the map name from the list
		ui_widget_t* list=g_interface.frames[0].widgets[0];
		ui_listcolumn_t* columns=(void*)ui_widgetget(list,UI_LIST_COLUMNS);
		int sel=(int)ui_widgetget(list,UI_LIST_SELECTION);
		if(sel<0)
			return;

		//Start new game
		g_destroygame();
		g_loadgame(g_interface.device,columns[1].items[sel]);

		//Don't forget to hide the cursor
		e_showmouse(g_interface.device,0);
	}

	//Close frame
	g_interface.frames[0].frame->flags&=~UI_FLAG_VISIBLE;
}

static void g_notifyaboutframe(ui_widget_t* widget,unsigned int code)
{
	//Close frame
	g_interface.frames[1].frame->flags&=~UI_FLAG_VISIBLE;
}

static void g_initgameframe(g_frame_t* frame)
{
	ui_widget_t* list,*button;
	ui_listcolumn_t* columns;

	frame->numwidgets=3;
	frame->widgets=(ui_widget_t**)sys_alloc(sizeof(ui_widget_t*)*frame->numwidgets);

	//Create the "new game" frame
	frame->frame=ui_createwidget(&ui_frameclass,0,0,400,300);
	frame->frame->render=&g_interface.render;
	frame->frame->caption="New game";
	frame->frame->flags&=~UI_FLAG_VISIBLE;

	//Create the map list
	list=ui_createwidget(&ui_listclass,5,26,390,238);
	list->render=&g_interface.render;

	//Two columns : "Map", "File"
	columns=(ui_listcolumn_t*)sys_alloc(sizeof(ui_listcolumn_t)*2);
	columns[0].width=250;
	columns[0].title="Maps";
	columns[0].numitems=0;
	columns[0].items=0;

	columns[1].width=list->width-2-columns[0].width;
	columns[1].title="File";
	columns[1].numitems=0;
	columns[1].items=0;

	ui_widgetset(list,UI_LIST_COLUMNCOUNT,(void*)2);
	ui_widgetset(list,UI_LIST_COLUMNS,(void*)columns);
	frame->widgets[0]=list;

	//Buttons : "Start" and "Cancel"
	button=ui_createwidget(&ui_buttonclass,320,270,75,25);
	button->render=&g_interface.render;
	button->caption="Start";
	button->type.notifyfunc=g_notifygameframe;
	button->user=(void*)1;
	frame->widgets[1]=button;

	button=ui_createwidget(&ui_buttonclass,5,270,75,25);
	button->render=&g_interface.render;
	button->caption="Cancel";
	button->type.notifyfunc=g_notifygameframe;
	button->user=(void*)0;
	frame->widgets[2]=button;

	ui_widgetset(frame->frame,UI_FRAME_WIDGETCOUNT,(void*)frame->numwidgets);
	ui_widgetset(frame->frame,UI_FRAME_WIDGETS,(void*)frame->widgets);
	g_centerframe(frame);
}

static void g_initaboutframe(g_frame_t* frame)
{
	ui_widget_t* text,*button;

	frame->numwidgets=2;
	frame->widgets=(ui_widget_t**)sys_alloc(sizeof(ui_widget_t*)*frame->numwidgets);

	//Create the "about" frame
	frame->frame=ui_createwidget(&ui_frameclass,0,0,300,100);
	frame->frame->render=&g_interface.render;
	frame->frame->caption="About";
	frame->frame->flags&=~UI_FLAG_VISIBLE;

	//Description and copyright
	text=ui_createwidget(&ui_textclass,10,40,100,50);
	text->caption="All In A Mouse's Night\nA Third Mouse Shooter Game\nCopyright(C) 2011";
	text->render=&g_interface.render;
	frame->widgets[0]=text;
	
	//Close button
	button=ui_createwidget(&ui_buttonclass,220,70,75,25);
	button->caption="Close";
	button->render=&g_interface.render;
	button->type.notifyfunc=g_notifyaboutframe;
	frame->widgets[1]=button;

	ui_widgetset(frame->frame,UI_FRAME_WIDGETCOUNT,(void*)frame->numwidgets);
	ui_widgetset(frame->frame,UI_FRAME_WIDGETS,(void*)frame->widgets);
	g_centerframe(frame);
}

static void g_initmainmenu(g_menu_t* menu)
{
	//Create the main menu
	menu->menu=ui_createwidget(&ui_menuclass,0,0,g_interface.device->width,0);
	menu->numitems=2;
	menu->items=(ui_menuitem_t*)sys_alloc(sizeof(ui_menuitem_t)*menu->numitems);
	menu->menu->render=&g_interface.render;

	//Two items: "Game" and "Help"
	menu->items[0].text="Game";
	menu->items[0].menu=0;

	menu->items[1].text="Help";
	menu->items[1].menu=0;

	ui_widgetset(menu->menu,UI_MENU_TYPE,(void*)UI_MENU_HORIZONTAL);
	ui_widgetset(menu->menu,UI_MENU_ITEMCOUNT,(void*)menu->numitems);
	ui_widgetset(menu->menu,UI_MENU_ITEMS,(void*)menu->items);
}

static void g_initgamemenu(g_menu_t* menu)
{
	//Create the game menu
	menu->numitems=3;
	menu->menu=ui_createwidget(&ui_menuclass,0,0,90,0);
	menu->items=(ui_menuitem_t*)sys_alloc(sizeof(ui_menuitem_t)*menu->numitems);
	menu->menu->render=&g_interface.render;

	//Three items: "New game", "Options", and "Quit"
	menu->items[0].text="New game";
	menu->items[0].menu=0;

	menu->items[1].text="Options";
	menu->items[1].menu=0;

	menu->items[2].text="Quit";
	menu->items[2].menu=0;

	menu->menu->type.notifyfunc=g_notifygamemenu;
	ui_widgetset(menu->menu,UI_MENU_ITEMCOUNT,(void*)menu->numitems);
	ui_widgetset(menu->menu,UI_MENU_ITEMS,(void*)menu->items);
}

static void g_inithelpmenu(g_menu_t* menu)
{
	//Create the help menu
	menu->numitems=1;
	menu->menu=ui_createwidget(&ui_menuclass,0,0,70,0);
	menu->items=(ui_menuitem_t*)sys_alloc(sizeof(ui_menuitem_t)*menu->numitems);
	menu->menu->render=&g_interface.render;

	//Only one item: "About"
	menu->items[0].text="About";
	menu->items[0].menu=0;

	menu->menu->type.notifyfunc=g_notifyhelpmenu;
	ui_widgetset(menu->menu,UI_MENU_ITEMCOUNT,(void*)menu->numitems);
	ui_widgetset(menu->menu,UI_MENU_ITEMS,(void*)menu->items);
}

static void g_linkmenus(void)
{
	g_menu_t* mainmenu=&g_interface.menus[0];
	//Link the "game" entry with the game menu
	mainmenu->items[0].menu=g_interface.menus[1].menu;
	//Link the "help" entry with the help menu
	mainmenu->items[1].menu=g_interface.menus[2].menu;
}

int g_initinterface(e_device_t* device)
{
	fs_handle_t file;

	memset(&g_interface,0,sizeof(g_interface_t));
	g_interface.device=device;
	g_interface.status=1;

	//Load interface font
	file=fs_open("data/uifont.ttf",FS_OPEN_READ);
	if(file==FS_INVALID_HANDLE)
		return 0;
	g_interface.render.font=e_loadfont(g_interface.device,G_INTERFACE_FONTSIZE,file);
	fs_close(file);
	if(!g_interface.render.font)
		return 0;

	g_interface.numframes=2;
	g_interface.frames=(g_frame_t*)sys_alloc(sizeof(g_frame_t)*g_interface.numframes);
	g_initgameframe(&g_interface.frames[0]);
	g_initaboutframe(&g_interface.frames[1]);

	g_interface.nummenus=3;
	g_interface.menus=(g_menu_t*)sys_alloc(sizeof(g_menu_t)*g_interface.nummenus);
	g_initmainmenu(&g_interface.menus[0]);
	g_initgamemenu(&g_interface.menus[1]);
	g_inithelpmenu(&g_interface.menus[2]);
	g_linkmenus();

	return 1;
}

void g_renderinterface(e_device_t* device)
{
	unsigned int i;

	e_applyortho(device);

	//No need to render the interface when the game is running
	if(g_flags&G_GAME_PAUSED)
	{
		for(i=0;i<g_interface.numframes;i++)
		{
			if(i==g_interface.focus)
				continue;
			ui_renderwidget(g_interface.frames[i].frame,device);
		}
		if(g_interface.numframes)
			ui_renderwidget(g_interface.frames[g_interface.focus].frame,device);
		if(g_interface.nummenus)
			ui_renderwidget(g_interface.menus[0].menu,device);
	}

	if(g_flags&G_GAME_CONSOLE)
		g_renderconsole(device,g_interface.render.font);
}

int g_eventinterface(e_event_t* event)
{
	//Switch from game states
	if(event->type==E_EVENT_KEYDOWN)
	{
		switch(event->keyboard.key)
		{
			case E_KEY_TAB:
				g_flags^=G_GAME_CONSOLE;
				break;
			case E_KEY_ESCAPE:
				g_flags^=G_GAME_PAUSED;
				e_showmouse(g_interface.device,g_flags&G_GAME_PAUSED);
				break;
		}
	}

	//The game is not paused, so the interface is not drawn
	if(!g_flags&G_GAME_PAUSED)
		return g_interface.status;
	
	if(g_interface.nummenus)
	{
		ui_eventwidget(g_interface.menus[0].menu,event);
		if((int)ui_widgetget(g_interface.menus[0].menu,UI_MENU_SELECTION)>0)
			return g_interface.status;
	}

	if(event->type==E_EVENT_MOUSEBUTTONDOWN||
		event->type==E_EVENT_MOUSEBUTTONUP)
	{
		unsigned int i;
		unsigned int selected=g_interface.focus;

		//Focus on another widget if needed
		for(i=0;i<g_interface.numframes;i++)
		{
			ui_widget_t* frame=g_interface.frames[i].frame;
			if(!(frame->flags&UI_FLAG_VISIBLE))
				continue;
			if(ui_isinside(frame,event->mouse.x,event->mouse.y))
			{
				selected=i;
				if(i==g_interface.focus)
					break;
			}
		}
		g_interface.focus=selected;
	}
	else
	{
		//Exit the program
		if(event->type==E_EVENT_QUIT)
		{
			g_interface.status=0;
			return 0;
		}
	}

	//Send this event to the focused widget
	if(g_interface.numframes)
		ui_eventwidget(g_interface.frames[g_interface.focus].frame,event);

	return g_interface.status;
}

void g_destroyinterface(void)
{
	unsigned int i,j;

	//Destroy menus
	if(g_interface.nummenus)
	{
		for(i=0;i<g_interface.nummenus;i++)
		{
			g_menu_t* menu=&g_interface.menus[i];
			ui_destroywidget(menu->menu);

			if(menu->items)
				sys_free(menu->items);
		}

		sys_free(g_interface.menus);
	}

	//Destroy frames
	if(g_interface.numframes)
	{
		for(i=0;i<g_interface.numframes;i++)
		{
			g_frame_t* frame=&g_interface.frames[i];
			ui_destroywidget(frame->frame);

			if(frame->widgets)
			{
				for(j=0;j<frame->numwidgets;j++)
				{
					//We need to release widget-related data
					g_destroywidget(frame->widgets[j]);
					ui_destroywidget(frame->widgets[j]);
				}
				sys_free(frame->widgets);
			}
		}

		sys_free(g_interface.frames);
	}

	//Destroy font
	e_destroyfont(g_interface.render.font);
}
