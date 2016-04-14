#include "ui_edit.h"

struct ui_editdata_s
{
	unsigned int maxtext;
	int caret;
	unsigned int start;
	unsigned int count;
	unsigned int* positions;
	unsigned int margin;
	int uppercase:1;
};

typedef struct ui_editdata_s ui_editdata_t;

static void ui_editcreate(ui_widget_t*);
static void ui_editdestroy(ui_widget_t*);
static void ui_editrender(ui_widget_t*,e_device_t*);
static void ui_editevent(ui_widget_t*,e_event_t*);
static void* ui_editget(ui_widget_t*,unsigned int);
static void ui_editset(ui_widget_t*,unsigned int,void*);

const ui_widgetclass_t ui_editclass={UI_WIDGET_EDIT,ui_editcreate,ui_editdestroy,ui_editrender,ui_editevent,ui_editget,ui_editset,0};

void ui_editcreate(ui_widget_t* widget)
{
	ui_editdata_t* data=(ui_editdata_t*)sys_alloc(sizeof(ui_editdata_t));
	memset(data,0,sizeof(ui_editdata_t));
	data->caret=-1;
	widget->data=data;
}

void ui_editdestroy(ui_widget_t* widget)
{
	ui_editdata_t* data=(ui_editdata_t*)widget->data;
	if(data->positions)
		sys_free(data->positions);
	sys_free(data);
}

void ui_editrender(ui_widget_t* widget,e_device_t* device)
{
	ui_editdata_t* data=(ui_editdata_t*)widget->data;
	e_renderframe(device,widget->x,widget->y,widget->width,widget->height,E_FRAME_SUNKEN);
	if(widget->caption&&widget->render&&data->positions)
	{
		const unsigned int margin=widget->render->font->maxheight/2;
		const unsigned int y=widget->y+(widget->height+widget->render->font->maxheight)/2-1;
		unsigned int i;
		unsigned int x=0;
		int filled=0;

		if(data->caret<0&&data->start>0)
			data->start--;

		memset(data->positions,0,sizeof(unsigned int)*(data->maxtext+2));
		for(i=data->start;i<strlen(widget->caption);i++)
		{
			const unsigned int index=widget->caption[i];
			e_fontcharacter_t* character=&widget->render->font->characters[index];
			data->positions[i]=x;
			x+=character->advance;
			if(x>=widget->width-2*margin)
			{
				filled=1;
				i++;
				break;
			}
		}
		data->positions[i]=x;
		data->positions[i+1]=x+margin;

		if(filled)
			i--;

		data->count=i-data->start;
		data->margin=margin+widget->x;

		if(filled&&data->caret>0&&data->caret>=(int)(data->start+data->count))
			data->start++;

		if(data->caret>=0)
			e_renderframe(device,data->margin+data->positions[data->caret]-data->positions[data->start],y-widget->render->font->maxheight,data->positions[data->caret+1]-data->positions[data->caret],widget->render->font->maxheight+1,E_FRAME_NOBORDER);
		e_printrange(device,widget->render->font,data->margin,y,widget->caption+data->start,data->count);
	}
}

void ui_editevent(ui_widget_t* widget,e_event_t* event)
{
	switch(event->type)
	{
		case E_EVENT_MOUSEBUTTONDOWN:
			if(ui_isinside(widget,event->mouse.x,event->mouse.y))
			{
				ui_editdata_t* data=(ui_editdata_t*)widget->data;
				const unsigned int margin=data->margin-data->positions[data->start];
				unsigned int i;

				data->caret=-1;
				for(i=data->start;i<data->start+data->count+1;i++)
				{
					if(event->mouse.x>=(int)(data->positions[i]+margin))
						data->caret=i;
					else
						break;
				}
			}
			else
			{
				ui_editdata_t* data=(ui_editdata_t*)widget->data;
				data->caret=-1;
			}
			break;
		case E_EVENT_KEYDOWN:
			{
				ui_editdata_t* data=(ui_editdata_t*)widget->data;
				if(!widget->caption||data->caret<0)
					return;
				switch(event->keyboard.key)
				{
					case E_KEY_DELETE:
						{
							const unsigned int size=strlen(widget->caption+data->caret);
							strncpy(widget->caption+data->caret,widget->caption+data->caret+1,size);
							if(size>0)
								widget->caption[data->caret+size-1]='\0';
						}
						break;
					case E_KEY_BACKSPACE:
						{
							if(data->caret>0)
							{
								const unsigned int size=strlen(widget->caption+data->caret);
								strncpy(widget->caption+data->caret-1,widget->caption+data->caret,size);
								widget->caption[data->caret+size-1]='\0';
								if(data->caret==data->start)
									data->start--;
								data->caret--;
							}
						}
						break;
					case E_KEY_LSHIFT:
					case E_KEY_RSHIFT:
						data->uppercase=1;
						break;
					case E_KEY_LEFT:
						if(data->caret>0)
						{
							if(data->caret<=(int)data->start)
								data->start--;
							data->caret--;
						}
						break;
					case E_KEY_RIGHT:
						if(data->caret<(int)(data->start+data->count))
							data->caret++;
						break;
					default:
						{
							const unsigned int len=strlen(widget->caption);
							if(!isascii(event->keyboard.key))
								break;
							if(len+1<data->maxtext)
							{
								unsigned int i;

								for(i=len;i>(unsigned int)data->caret;i--)
									widget->caption[i]=widget->caption[i-1];
								widget->caption[i]=(data->uppercase)?toupper(event->keyboard.key):event->keyboard.key;

								data->caret++;
							}
						}
						break;
				}
			}
			break;
		case E_EVENT_KEYUP:
			if(event->keyboard.key==E_KEY_LSHIFT||event->keyboard.key==E_KEY_RSHIFT)
			{
				ui_editdata_t* data=(ui_editdata_t*)widget->data;
				data->uppercase=0;
			}
			break;
	}
}

void* ui_editget(ui_widget_t* widget,unsigned int index)
{
	if(index==UI_EDIT_MAXTEXT)
	{
		ui_editdata_t* data=(ui_editdata_t*)widget->data;
		return (void*)data->maxtext;
	}
	return 0;
}

void ui_editset(ui_widget_t* widget,unsigned int index,void* param)
{
	if(index==UI_EDIT_MAXTEXT)
	{
		ui_editdata_t* data=(ui_editdata_t*)widget->data;
		data->maxtext=(unsigned int)param;
		if(data->positions)
			sys_free(data->positions);
		data->positions=(unsigned int*)sys_alloc(sizeof(unsigned int)*(data->maxtext+2));
	}
}
