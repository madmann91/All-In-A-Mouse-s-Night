#include "config.h"
#include "e_device.h"
#include "e_programs.h"

PFNGLGENBUFFERSARBPROC glGenBuffersARB;
PFNGLDELETEBUFFERSARBPROC glDeleteBuffersARB;
PFNGLBINDBUFFERARBPROC glBindBufferARB;
PFNGLBUFFERDATAARBPROC glBufferDataARB;
PFNGLMAPBUFFERARBPROC glMapBufferARB;
PFNGLUNMAPBUFFERARBPROC glUnmapBufferARB;
/*PFNGLMULTITEXCOORD1FARBPROC glMultiTexCoord1fARB;
PFNGLMULTITEXCOORD2FARBPROC glMultiTexCoord2fARB;
PFNGLACTIVETEXTUREARBPROC glActiveTextureARB;
PFNGLCLIENTACTIVETEXTUREARBPROC glClientActiveTextureARB;*/
PFNGLBINDPROGRAMARBPROC glBindProgramARB;
PFNGLDELETEPROGRAMSARBPROC glDeleteProgramsARB;
PFNGLGENPROGRAMSARBPROC glGenProgramsARB;
PFNGLPROGRAMSTRINGARBPROC glProgramStringARB;
PFNGLGETPROGRAMIVARBPROC glGetProgramivARB;
PFNGLPROGRAMENVPARAMETER4FVARBPROC glProgramEnvParameter4fvARB;
PFNGLACTIVESTENCILFACEEXTPROC glActiveStencilFaceEXT;

static int e_findextensions(void)
{
	int numunits;

	//Vertex buffer objects
	if(!(glGenBuffersARB=(PFNGLGENBUFFERSPROC)SDL_GL_GetProcAddress("glGenBuffersARB")))
		return 0;
	if(!(glDeleteBuffersARB=(PFNGLDELETEBUFFERSPROC)SDL_GL_GetProcAddress("glDeleteBuffersARB")))
		return 0;
	if(!(glBindBufferARB=(PFNGLBINDBUFFERPROC)SDL_GL_GetProcAddress("glBindBufferARB")))
		return 0;
	if(!(glBufferDataARB=(PFNGLBUFFERDATAPROC)SDL_GL_GetProcAddress("glBufferDataARB")))
		return 0;
	if(!(glMapBufferARB=(PFNGLMAPBUFFERPROC)SDL_GL_GetProcAddress("glMapBufferARB")))
		return 0;
	if(!(glUnmapBufferARB=(PFNGLUNMAPBUFFERPROC)SDL_GL_GetProcAddress("glUnmapBufferARB")))
		return 0;

	//Multitexturing
	/*if(!(glMultiTexCoord1fARB=(PFNGLMULTITEXCOORD1FPROC)SDL_GL_GetProcAddress("glMultiTexCoord1fARB")))
		return 0;
	if(!(glMultiTexCoord2fARB=(PFNGLMULTITEXCOORD2FPROC)SDL_GL_GetProcAddress("glMultiTexCoord2fARB")))
		return 0;
	if(!(glActiveTextureARB=(PFNGLACTIVETEXTUREPROC)SDL_GL_GetProcAddress("glActiveTextureARB")))
		return 0;
	if(!(glClientActiveTextureARB=(PFNGLCLIENTACTIVETEXTUREPROC)SDL_GL_GetProcAddress("glClientActiveTextureARB")))
		return 0;*/

	glGetIntegerv(GL_MAX_TEXTURE_UNITS_ARB,&numunits);
	if(numunits<4)
		return 0;

	//Vertex program
	if(!(glBindProgramARB=(PFNGLBINDPROGRAMARBPROC)SDL_GL_GetProcAddress("glBindProgramARB")))
		return 0;
	if(!(glDeleteProgramsARB=(PFNGLDELETEPROGRAMSARBPROC)SDL_GL_GetProcAddress("glDeleteProgramsARB")))
		return 0;
	if(!(glGenProgramsARB=(PFNGLGENPROGRAMSARBPROC)SDL_GL_GetProcAddress("glGenProgramsARB")))
		return 0;
	if(!(glProgramStringARB=(PFNGLPROGRAMSTRINGARBPROC)SDL_GL_GetProcAddress("glProgramStringARB")))
		return 0;
	if(!(glGetProgramivARB=(PFNGLGETPROGRAMIVARBPROC)SDL_GL_GetProcAddress("glGetProgramivARB")))
		return 0;
	if(!(glProgramEnvParameter4fvARB=(PFNGLPROGRAMENVPARAMETER4FVARBPROC)SDL_GL_GetProcAddress("glProgramEnvParameter4fvARB")))
		return 0;

	//Two sided stencil buffer
	glActiveStencilFaceEXT=(PFNGLACTIVESTENCILFACEEXTPROC)SDL_GL_GetProcAddress("glActiveStencilFaceEXT");

	return 1;
}

e_device_t* e_createdevice(const char* caption,unsigned int width,unsigned int height,unsigned int bpp,int fullscreen)
{
	e_device_t* device;
	SDL_Surface* icon;
	FT_Library library;

	if(FT_Init_FreeType(&library))
		return 0;

	if(!(SDL_WasInit(SDL_INIT_VIDEO)&SDL_INIT_VIDEO))
	{
		if(SDL_Init(SDL_INIT_VIDEO)<0)
			return 0;
	}

	SDL_GL_SetAttribute(SDL_GL_RED_SIZE,5);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE,5);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE,5);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE,16);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE,8);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER,1);

	SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY,SDL_DEFAULT_REPEAT_INTERVAL);
	SDL_WM_SetCaption(caption,NULL);
	icon=SDL_LoadBMP("icon.bmp");
	if(icon)
	{
		SDL_SetColorKey(icon,SDL_SRCCOLORKEY,SDL_MapRGB(icon->format,0,255,0));	
		SDL_WM_SetIcon(icon,0);
		SDL_FreeSurface(icon);
	}	
	SDL_SetVideoMode(width,height,bpp,(fullscreen)?SDL_FULLSCREEN|SDL_OPENGL:SDL_OPENGL);

	if(!e_findextensions())
		return 0;

	e_registerprograms();

	device=(e_device_t*)sys_alloc(sizeof(e_device_t));
	memset(device,0,sizeof(e_device_t));

	memcpy(&device->library,&library,sizeof(FT_Library));
	device->active=1;
	device->fullscreen=(fullscreen==1);

	device->width=width;
	device->height=height;
	device->bpp=bpp;

	return device;
}

void e_destroydevice(e_device_t* device)
{
	FT_Done_FreeType(device->library);
	e_unregisterprograms();
	sys_free(device);
	SDL_Quit();
}

void e_getmouse(e_device_t* device,int* x,int* y)
{
	SDL_GetMouseState(x,y);
}

void e_setmouse(e_device_t* device,int x,int y)
{
	SDL_WarpMouse(x,y);
}

void e_showmouse(e_device_t* device,int show)
{
	SDL_ShowCursor(show);
}

const unsigned char* e_getkeyboard(e_device_t* device)
{
	return SDL_GetKeyState(NULL);
}

int e_getevent(e_device_t* device,e_event_t* event)
{
	static const unsigned int buttons[]={E_BUTTON_NONE,E_BUTTON_LEFT,E_BUTTON_MIDDLE,E_BUTTON_RIGHT};
	SDL_Event sdlevent;
	while(SDL_PollEvent(&sdlevent))
	{
		switch(sdlevent.type)
		{
			case SDL_ACTIVEEVENT:
				if(sdlevent.active.state&SDL_APPACTIVE)
					device->active=sdlevent.active.gain;
				break;
			case SDL_KEYDOWN:
				event->type=E_EVENT_KEYDOWN;
				event->keyboard.key=sdlevent.key.keysym.sym;
				return 1;
			case SDL_KEYUP:
				event->type=E_EVENT_KEYUP;
				event->keyboard.key=sdlevent.key.keysym.sym;
				return 1;
			case SDL_MOUSEMOTION:
				event->type=E_EVENT_MOUSEMOVE;
				event->mouse.x=sdlevent.motion.x;
				event->mouse.y=sdlevent.motion.y;
				event->mouse.xrel=sdlevent.motion.xrel;
				event->mouse.yrel=sdlevent.motion.yrel;
				event->mouse.button=E_BUTTON_NONE;
				return 1;
			case SDL_MOUSEBUTTONDOWN:
				event->type=E_EVENT_MOUSEBUTTONDOWN;
				event->mouse.x=sdlevent.button.x;
				event->mouse.y=sdlevent.button.y;
				event->mouse.xrel=0;
				event->mouse.yrel=0;
				event->mouse.button=buttons[sdlevent.button.button];
				return 1;
			case SDL_MOUSEBUTTONUP:
				event->type=E_EVENT_MOUSEBUTTONUP;
				event->mouse.x=sdlevent.button.x;
				event->mouse.y=sdlevent.button.y;
				event->mouse.xrel=0;
				event->mouse.yrel=0;
				event->mouse.button=buttons[sdlevent.button.button];
				return 1;
			case SDL_QUIT:
				event->type=E_EVENT_QUIT;
				return 1;
		}
	}
	return 0;
}

void e_cleardevice(e_device_t* device)
{
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
}

void e_renderdevice(e_device_t* device)
{
	glFlush();
	glFinish();
	SDL_GL_SwapBuffers();
}

void e_renderframe(e_device_t* device,int x,int y,unsigned int width,unsigned int height,int flags)
{
	static const mt_scalar_t colors[4][4]={{0.3f,0.3f,0.3f,0.65f},
					{0.4f,0.4f,0.4f,0.65f},
					{0.5f,0.5f,0.5f,0.75f},
					{0.2f,0.2f,0.2f,0.75f}};

	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);

	glPushMatrix();
	glTranslatef((mt_scalar_t)x,(mt_scalar_t)y,0.0f);

	if(flags&E_FRAME_SUNKEN)
		glColor4fv(colors[0]);
	else
		glColor4fv(colors[1]);

	glBegin(GL_QUADS);
	glVertex2i(0,0);
	glVertex2i(width,0);
	glVertex2i(width,height);
	glVertex2i(0,height);
	glEnd();

	if((flags&E_FRAME_NOBORDER)!=E_FRAME_NOBORDER)
	{
		glBegin(GL_LINES);
		if(flags&E_FRAME_BUMP)
			glColor4fv(colors[2]);
		else
			glColor4fv(colors[3]);
		if(!(flags&E_FRAME_NOTOP))
		{
			glVertex2i(0,0);
			glVertex2i(width,0);
		}
		if(!(flags&E_FRAME_NOLEFT))
		{
			glVertex2i(0,0);
			glVertex2i(0,height);
		}
		if(flags&E_FRAME_BUMP)
			glColor4fv(colors[3]);
		else
			glColor4fv(colors[2]);
		if(!(flags&E_FRAME_NOBOTTOM))
		{
			glVertex2i(0,height);
			glVertex2i(width,height);
		}
		if(!(flags&E_FRAME_NORIGHT))
		{
			glVertex2i(width,0);
			glVertex2i(width,height);
		}
		glEnd();
	}

	glPopMatrix();
	glDisable(GL_BLEND);
	glColor3f(1.0f,1.0f,1.0f);
}

void e_fade(e_device_t* device,mt_scalar_t factor)
{
	glEnable(GL_BLEND);
	glColor4f(0,0,0,(mt_scalar_t)1.0-factor);

	glBegin(GL_QUADS);
	glVertex2i(0,0);
	glVertex2i(device->width,0);
	glVertex2i(device->width,device->height);
	glVertex2i(0,device->height);
	glEnd();

	glColor4f(1,1,1,1);
	glDisable(GL_BLEND);
}
