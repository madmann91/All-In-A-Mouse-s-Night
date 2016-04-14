#include "sys_common.h"

#include "g_common.h"
#include "g_console.h"
#include "g_variables.h"
#include "g_interface.h"
#include "g_game.h"

#include "e_device.h"
#include "e_utils.h"

static e_device_t* g_device;

int g_init(void)
{
	//This must be called before any other function
	if(!sys_init())
		return 0;

	g_log(G_LOG_NOHEADER,"All In A Mouse's Night v1.0 - Copyright(C) 2010-2011\n"
		"This is the game log file : send it with a bug report\n");

	//Device creation
	g_device=e_createdevice("All In A Mouse's Night",1920,1080,32,0);
	if(!g_device)
	{
		g_log(G_LOG_ERROR,"Cannot create device, maybe a problem with OpenGL extensions");
		g_log(G_LOG_MESSAGE,"This is your OpenGL extension string : %s",glGetString(GL_EXTENSIONS));
		return 0;
	}

	//File system initialisation
	if(!fs_init())
	{
		g_log(G_LOG_ERROR,"Cannot initialize file system, check your installation");
		return 0;
	}

	//Console initialisation
	g_initconsole(g_device,80,20);
	if(!g_initinterface(g_device))
	{
		g_log(G_LOG_ERROR,"Cannot initialize interface");
		return 0;
	}

	//Load a default game
	if(!g_loadgame(g_device,"tea"))
	{
		g_log(G_LOG_ERROR,"Cannot load default game, check your installation");
		return 0;
	}

	e_showmouse(g_device,g_flags&G_GAME_PAUSED);
	
	return 1;
}

void g_run(void)
{
	unsigned long ticks;
	mt_scalar_t dt=(mt_scalar_t)0.0;

	while(1)
	{
		e_event_t event;
		ticks=e_getticks();

		//Send events to interface
		while(e_getevent(g_device,&event))
		{
			if(!g_eventinterface(&event))
				return;
		}

		e_cleardevice(g_device);
		g_updategame(g_device,dt);
		g_rendergame(g_device);
		g_renderinterface(g_device);
		e_renderdevice(g_device);

		while(e_getticks()-ticks<g_delay)
			e_wait(1);

		dt=g_delay*(mt_scalar_t)0.001;
	}
}

void g_quit(void)
{
	//Destroy game objects
	g_destroyinterface();
	g_destroygame();
	g_destroyconsole();

	//Destroy device
	e_destroydevice(g_device);

	//Destroy file system
	fs_quit();

	//Last function called
	sys_quit();
}
