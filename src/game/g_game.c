#include "sys_common.h"

#include "e_utils.h"

#include "g_game.h"
#include "g_variables.h"
#include "g_math.h"
#include "g_console.h"

static g_game_t g_game;

static void g_readinfo(const char* description)
{
	unsigned int w,h;
	char* ptr,*next;

	//Find title
	ptr=(char*)strchr(description,'\n');
	if(ptr)
		*ptr=0;

	g_game.numlines=0;
	g_game.description=0;
	g_game.title=(char*)sys_alloc((strlen(description)+1)*sizeof(char));
	strcpy(g_game.title,description);

	if(!ptr)
		return;

	ptr=strchr(ptr+1,'\n');
	if(ptr)
		*ptr=0;

	//Find description lines and compute the dimensions of the rendered text
	g_game.maxwidth=0;
	while(ptr!=0)
	{
		ptr++;
		next=(char*)strchr(ptr+1,'\n');
		if(next)
			*next=0;

		g_game.numlines++;
		g_game.description=(char**)sys_realloc(g_game.description,sizeof(char*)*g_game.numlines);
		g_game.description[g_game.numlines-1]=(char*)sys_alloc((strlen(ptr)+1)*sizeof(char));
		strcpy(g_game.description[g_game.numlines-1],ptr);

		e_textsize(g_game.fonts[G_FONT_MEDIUM],ptr,&w,&h);
		g_game.maxwidth=(g_game.maxwidth<w)?w:g_game.maxwidth;
		ptr=next;
	}
}

int g_loadgame(e_device_t* device,const char* world)
{
	fs_handle_t file;
	unsigned int i;

	//Setup variables
	g_reset();

	g_log(G_LOG_MESSAGE,"Loading game \"%s\"",world);
	
	g_game.numparticles=0;

	//Load game fonts
	file=fs_open("data\\gamefont.ttf",FS_OPEN_READ);
	if(file==FS_INVALID_HANDLE)
	{
		g_log(G_LOG_ERROR,"Cannot open game font file");
		return 0;
	}
	for(i=0;i<G_FONT_MAX;i++)
	{
		static const unsigned int sizes[]={10,14,18};

		g_game.fonts[i]=e_loadfont(device,sizes[i],file);
		if(!g_game.fonts[i])
		{
			fs_close(file);
			g_log(G_LOG_ERROR,"Cannot load game font file with size %d",sizes[i]);
			return 0;
		}

		fs_seek(file,0,FS_SEEK_SET);
	}
	fs_close(file);

	//We first try to load the world
	g_game.world=g_loadworld(device,world);
	if(!g_game.world)
	{
		g_log(G_LOG_ERROR,"Cannot load world \"%s\"",world);
		return 0;
	}

	//Load game script (required for every world)
	file=fs_open(fs_pathtofile(world,"game.lua"),FS_OPEN_READ);
	if(file==FS_INVALID_HANDLE)
	{
		g_log(G_LOG_ERROR,"Cannot open game script");
		return 0;
	}
	g_game.script=g_loadscript(device,g_game.world,file);
	fs_close(file);
	if(!g_game.script)
	{
		g_log(G_LOG_ERROR,"Cannot load game script");
		return 0;
	}

	//Load the game description (optional)
	file=fs_open(fs_pathtofile(world,"info.txt"),FS_OPEN_READ);
	if(file==FS_INVALID_HANDLE)
	{
		g_log(G_LOG_WARNING,"Cannot load game description");
		g_game.title=(char*)sys_alloc(sizeof(char)*16);
		strcpy(g_game.title,"Untitled");
		g_game.description=0;
		g_game.numlines=0;
	}
	else
	{
		char* description;
		unsigned long size;

		//Read the whole file and parse it
		size=fs_size(file);
		description=sys_alloc(sizeof(char)*(size+1));
		description[size]=0;
		fs_read(description,size,file);
		g_readinfo(description);
		sys_free(description);
		fs_close(file);
	}

	//Prepare camera (if the loading script fails to setup camera)
	g_lookat=-1;
	mt_vec3set(g_camera[0],(mt_scalar_t)0.0,(mt_scalar_t)10.0,(mt_scalar_t)0.0);
	mt_vec3set(g_camera[1],(mt_scalar_t)0.0,(mt_scalar_t)0.0,(mt_scalar_t)0.0);

	//Execute loading script...
	if(!g_callscript(g_game.script,"g_init"))
	{
		g_log(G_LOG_ERROR,"Cannot execute g_init");
		return 0;
	}

	//Prepare timer, and start fading in
	g_flags|=G_GAME_FADEIN;
	g_game.timer.elapsed=0;
	e_start(&g_game.timer);

	return 1;
}

void g_updategame(e_device_t* device,mt_scalar_t dt)
{
	unsigned int i;
	mt_vector3_t gravity={0,-g_gravity,0};

	//Prevent the timer from counting when the game is paused
	if(g_flags&G_GAME_PAUSED)
	{
		if(!g_game.timer.stopped)
			e_stop(&g_game.timer);
		return;
	}

	//Handle fading
	if(g_flags&G_GAME_FADEIN)
	{
		e_restart(&g_game.timer);
		if(g_game.timer.elapsed>=G_FADE_DELAY)
			g_flags^=G_GAME_FADEIN;
	}

	g_updateworld(g_game.world,dt);
	for(i=0;i<g_game.numparticles;i++)
		e_updateparticles(g_game.particles[i],gravity,0,dt);

	//Force the mouse position to the center of the screen
	e_setmouse(device,device->width/2,device->height/2);

	if(g_lookat>=0)
		g_charactercamera(g_camera[0],g_camera[1],&g_game.world->characters[g_lookat],g_game.world->mesh);
	
	e_setcamera(device,&g_game.camera,g_camera[0],g_camera[1],(mt_scalar_t)150.0,(mt_scalar_t)60.0);
}

void g_rendergame(e_device_t* device)
{
	unsigned int i;

	e_applycamera(device,&g_game.camera);

	g_renderworld(device,g_game.world,&g_game.camera);
	for(i=0;i<g_game.numparticles;i++)
		e_renderparticles(device,g_game.particles[i]);

	e_applyortho(device);
	
	//While fading in, we display the game description 
	if(g_flags&G_GAME_FADEIN)
	{
		unsigned int i,y,w,h;

		e_applyortho(device);

		e_fade(device,(mt_scalar_t)g_game.timer.elapsed/(mt_scalar_t)G_FADE_DELAY);

		e_textsize(g_game.fonts[G_FONT_BIG],g_game.title,&w,&h);
		e_print(device,g_game.fonts[G_FONT_BIG],(device->width-w)/2,device->height/3,g_game.title);

		y=device->height/3+g_game.fonts[G_FONT_BIG]->maxheight*2;
		for(i=0;i<g_game.numlines;i++)
		{
			e_print(device,g_game.fonts[G_FONT_MEDIUM],(device->width-g_game.maxwidth)/2,y,g_game.description[i]);
			y+=g_game.fonts[G_FONT_MEDIUM]->maxheight+1;
		}
	}
}

void g_destroygame(void)
{
	unsigned int i;

	//Call the exit function and destroy the script
	if(g_game.script)
	{
		g_callscript(g_game.script,"g_quit");
		g_destroyscript(g_game.script);
	}

	//Destroy the game description
	if(g_game.description)
	{
		for(i=0;i<g_game.numlines;i++)
			sys_free(g_game.description[i]);
		sys_free(g_game.description);
	}
	if(g_game.title)
		sys_free(g_game.title);

	//Destroy the engine objects...
	if(g_game.numparticles)
	{
		for(i=0;i<g_game.numparticles;i++)
			e_destroyparticles(g_game.particles[i]);
		sys_free(g_game.particles);
	}

	//... And the game objects too (they've bee created by the script object)
	if(g_game.world->charactertypes)
	{
		for(i=0;i<g_game.world->numcharactertypes;i++)
			g_destroycharacterclass(&g_game.world->charactertypes[i]);
		sys_free(g_game.world->charactertypes);
	}
	if(g_game.world->characters)
	{
		for(i=0;i<g_game.world->numcharacters;i++)
			g_destroycharacter(&g_game.world->characters[i]);
		sys_free(g_game.world->characters);
	}
	if(g_game.world->weapons)
	{
		for(i=0;i<g_game.world->numweapons;i++)
			g_destroyweapon(&g_game.world->weapons[i]);
		sys_free(g_game.world->weapons);
	}

	//Destroy fonts
	for(i=0;i<G_FONT_MAX;i++)
	{
		if(g_game.fonts[i])
			e_destroyfont(g_game.fonts[i]);
	}

	//And finally, destroy the world
	if(g_game.world)
		g_destroyworld(g_game.world);
}