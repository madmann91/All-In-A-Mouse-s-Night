#include "sys_common.h"
#include "g_console.h"

static g_console_t g_console;

void g_log(unsigned int type,const char* format,...)
{
	static const char* header[]={0,"Warning","Error","Message"};
	static const char* days[7]={"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};
	static const char* months[12]={"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};

	va_list args;

	//Print date/time info
	if(type)
	{
		struct tm* hdtm;
		time_t hdtime;

		time(&hdtime);
		hdtm=localtime(&hdtime);
		printf("[%s %s %d %d:%d:%d] %s : ",days[hdtm->tm_wday],months[hdtm->tm_mon],hdtm->tm_mday,hdtm->tm_hour,hdtm->tm_min,hdtm->tm_sec,header[type]);
	}

	//Print message
	va_start(args,format);
	vprintf(format,args);
	va_end(args);
	printf("\n");
}

void g_initconsole(e_device_t* device,unsigned int width,unsigned int height)
{
	unsigned int i;

	g_console.width=width;
	g_console.height=height;
	g_console.size=2*width;
	g_console.line=(char*)sys_alloc(sizeof(char)*g_console.size);

	g_console.buffer=(char**)sys_alloc(sizeof(char*)*height);
	for(i=0;i<height;i++)
	{
		g_console.buffer[i]=(char*)sys_alloc(sizeof(char)*width);
		memset(g_console.buffer[i],0,sizeof(char)*width);
	}
	g_console.caret[0]=g_console.caret[1]=0;
}

void g_print(const char* format,...)
{
	const unsigned int maxwidth=g_console.width-1;
	const unsigned int maxheight=g_console.height-1;

	int len,i;
	va_list args;

	va_start(args,format);
	len=vsnprintf(g_console.line,g_console.size-1,format,args);

	//Ensure there is enough place for the text... (should be okay after 3 iterations in most cases)
	while(len==-1)
	{
		g_console.size*=2;
		g_console.line=(char*)sys_realloc(g_console.line,g_console.size*sizeof(char));
		va_start(args,format);
		len=vsnprintf(g_console.line,g_console.size-1,format,args);
	}

	va_end(args);

	for(i=0;i<len;i++)
	{
		//Do we need to add a new line ?
		if(g_console.line[i]=='\n'||g_console.caret[1]>=maxwidth)
		{
			g_console.buffer[g_console.caret[0]][g_console.caret[1]]=0;
			g_console.caret[0]++;
			g_console.caret[1]=0;
			if(g_console.caret[0]>=g_console.height)
			{
				unsigned int j;

				//Scroll console
				char* temp=g_console.buffer[0];
				for(j=0;j<maxheight;j++)
					g_console.buffer[j]=g_console.buffer[j+1];
				g_console.buffer[maxheight]=temp;

				memset(g_console.buffer[maxheight],0,sizeof(char)*g_console.width);
				g_console.caret[0]=maxheight;
			}
		}

		if(g_console.line[i]!='\n')
		{
			g_console.buffer[g_console.caret[0]][g_console.caret[1]]=g_console.line[i];
			g_console.caret[1]++;
		}
	}
}

void g_clear(void)
{
	unsigned int i;
	for(i=0;i<g_console.height;i++)
		memset(g_console.buffer[i],0,sizeof(char)*g_console.width);
	g_console.caret[0]=g_console.caret[1]=0;
}

void g_renderconsole(e_device_t* device,e_font_t* font)
{
	unsigned int i,y=0;
	e_renderframe(device,1,1,device->width-1,font->maxheight*g_console.height+font->maxheight/2,E_FRAME_SUNKEN|E_FRAME_BUMP);
	
	for(i=0;i<g_console.height;i++)
	{
		y+=font->maxheight;
		e_print(device,font,0,y,g_console.buffer[i]);
	}
}

void g_destroyconsole(void)
{
	unsigned int i;

	sys_free(g_console.line);
	for(i=0;i<g_console.height;i++)
		sys_free(g_console.buffer[i]);
	sys_free(g_console.buffer);
}
