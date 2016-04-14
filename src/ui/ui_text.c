#include "ui_text.h"

static void ui_textcreate(ui_widget_t*);
static void ui_textdestroy(ui_widget_t*);
static void ui_textrender(ui_widget_t*,e_device_t*);
static void ui_textevent(ui_widget_t*,e_event_t*);
static void* ui_textget(ui_widget_t*,unsigned int);
static void ui_textset(ui_widget_t*,unsigned int,void*);

const ui_widgetclass_t ui_textclass={UI_WIDGET_TEXT,ui_textcreate,ui_textdestroy,ui_textrender,ui_textevent,ui_textget,ui_textset,0};

void ui_textcreate(ui_widget_t* widget)
{

}

void ui_textdestroy(ui_widget_t* widget)
{

}

void ui_textrender(ui_widget_t* widget,e_device_t* device)
{
	if(widget->caption&&widget->render)
	{
		char* ptr=widget->caption;
		char* end=strchr(widget->caption,'\n');
		unsigned int y=widget->y;
		while(end)
		{
			e_printrange(device,widget->render->font,widget->x,y,ptr,end-ptr);
			ptr=end+1;
			y+=widget->render->font->maxheight;
			end=strchr(ptr,'\n');
		}
		e_print(device,widget->render->font,widget->x,y,ptr);
	}
}

void ui_textevent(ui_widget_t* widget,e_event_t* event)
{

}

void* ui_textget(ui_widget_t* widget,unsigned int index)
{
	return 0;
}

void ui_textset(ui_widget_t* widget,unsigned int index,void* param)
{

}
