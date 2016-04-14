#include "sys_common.h"
#include "g_script.h"
#include "g_console.h"
#include "g_variables.h"

#define G_SCRIPT_KEY "g_script"

#define G_READTABLE(s,t,v,n) \
	{ \
		unsigned int i; \
		for(i=0;i<n;i++) \
		{ \
			lua_pushinteger(s,i+1); \
			lua_gettable(s,t); \
			v[i]=(mt_scalar_t)lua_tonumber(s,-1); \
		} \
	}

struct g_scriptreader_s
{
	char* buffer;
	unsigned int size;
	fs_handle_t file;
};

typedef struct g_scriptreader_s g_scriptreader_t;

static lua_State* lua_open(void) {
	return luaL_newstate();
}

static const char* g_readchunk(lua_State* state,void* data,size_t* size)
{
	g_scriptreader_t* reader=(g_scriptreader_t*)data;
	unsigned int count;
	count=fs_read(reader->buffer,reader->size,reader->file);
	*size=count;
	return reader->buffer;
}

g_script_t* g_loadscript(e_device_t* device,g_world_t* world,fs_handle_t file)
{
	lua_State* state;
	g_scriptreader_t reader;
	g_script_t* script;
	int res;
	
	state=lua_open();
	if(!state)
		return 0;

	script=(g_script_t*)sys_alloc(sizeof(g_script_t));
	memset(script,0,sizeof(g_script_t));

	script->device=device;
	script->world=world;
	script->state=state;

	//Register functions
	lua_register(state,"g_loadlibrary",__g_loadlibrary);

	lua_register(state,"g_registerweapon",__g_registerweapon);
	lua_register(state,"g_addweapon",__g_addweapon);

	lua_register(state,"g_registercharacter",__g_registercharacter);
	lua_register(state,"g_addcharacter",__g_addcharacter);
	lua_register(state,"g_removecharacter",__g_removecharacter);

	lua_register(state,"g_lookat",__g_lookat);
	lua_register(state,"g_physics",__g_physics);
	lua_register(state,"g_print",__g_print);

	//Make the script object visible from script functions
	lua_pushstring(state,G_SCRIPT_KEY);
	lua_pushlightuserdata(state,script);
	lua_settable(state,LUA_REGISTRYINDEX);

	reader.buffer=sys_alloc(sizeof(char)*256);
	reader.size=256;
	reader.file=file;

	res=lua_load(state,g_readchunk,&reader,"g_script","bt")||lua_pcall(state,0,0,0);

	sys_free(reader.buffer);
	if(res)
	{
		sys_free(script);
		lua_close(state);
		return 0;
	}

	return script;
}

int g_callscript(g_script_t* script,const char* name)
{
	lua_getglobal(script->state,name);
	if(lua_pcall(script->state,0,0,0))
	{
		g_log(G_LOG_ERROR,"Lua says \"%s\"",lua_tostring(script->state,1));
		return 0;
	}
	return 1;
}

void g_destroyscript(g_script_t* script)
{
	lua_close(script->state);
	sys_free(script);
}

int __g_loadlibrary(lua_State* state)
{
	int nargs=lua_gettop(state);
	g_scriptreader_t reader;
	const char* name;
	int res;

	g_script_t* script;

	if(nargs!=1)
	{
		g_print("Error : Wrong number of arguments to g_loadlibrary\n");
		return 0;
	}

	//Retreive script object from registry
	lua_pushstring(state,G_SCRIPT_KEY);
	lua_gettable(state,LUA_REGISTRYINDEX);
	script=(g_script_t*)lua_touserdata(state,-1);

	//Load library from game package
	name=lua_tostring(state,1);

	reader.file=fs_open(name,FS_OPEN_READ);
	if(reader.file==FS_INVALID_HANDLE)
	{
		g_print("Error : Cannot open library \"%s\"\n",name);
		return 0;
	}
	reader.buffer=sys_alloc(sizeof(char)*256);
	reader.size=256;

	res=lua_load(state,g_readchunk,&reader,name,"bt")||lua_pcall(state,0,0,0);
	sys_free(reader.buffer);
	fs_close(reader.file);
	if(res)
	{
		g_print("Error : Cannot load library \"%s\"\n",name);
		return 0;
	}

	return 0;
}

int __g_registerweapon(lua_State* state)
{
	int nargs=lua_gettop(state);
	g_weapon_t* weapon;
	g_script_t* script;
	g_world_t* world;

	fs_handle_t file;
	e_model_t* model;
	e_texture_t* texture;

	if(nargs!=4)
	{
		g_print("Error : Wrong number of arguments to g_registerweapon\n");
		return 0;
	}

	//Retreive script object from registry
	lua_pushstring(state,G_SCRIPT_KEY);
	lua_gettable(state,LUA_REGISTRYINDEX);
	script=(g_script_t*)lua_touserdata(state,-1);
	world=script->world;

	//Try to load weapon model
	file=fs_open(lua_tostring(state,3),FS_OPEN_READ);
	if(file==FS_INVALID_HANDLE)
	{
		g_print("Error : Cannot open weapon model file \"%s\"\n",lua_tostring(state,3));
		return 0;
	}
	model=e_loadmodel(script->device,file);
	fs_close(file);
	if(!model)
	{
		g_print("Error : Invalid weapon model file \"%s\"\n",lua_tostring(state,3));
		return 0;
	}

	//Try to load weapon texture
	file=fs_open(lua_tostring(state,4),FS_OPEN_READ);
	if(file==FS_INVALID_HANDLE)
	{
		e_destroymodel(model);
		g_print("Error : Cannot open weapon texture file \"%s\"\n",lua_tostring(state,4));
		return 0;
	}
	texture=e_loadtexture(script->device,0,file);
	fs_close(file);
	if(!texture)
	{
		e_destroymodel(model);
		g_print("Error : Invalid weapon texture file \"%s\"\n",lua_tostring(state,4));
		return 0;
	}

	//Register weapon into the world object
	world->numweapons++;
	world->weapons=(g_weapon_t*)sys_realloc(world->weapons,sizeof(g_weapon_t)*world->numweapons);
	weapon=&world->weapons[world->numweapons-1];

	strncpy(weapon->name,lua_tostring(state,1),G_MAX_WEAPONNAME);
	weapon->effect=G_EFFECT_BULLET;
	weapon->model=model;
	weapon->texture=texture;
	weapon->buffer=e_createmodelbuffer(weapon->model);

	return 0;
}

int __g_addweapon(lua_State* state)
{
	int nargs=lua_gettop(state);
	unsigned int i,j;
	const char* name;
	g_script_t* script;
	g_world_t* world;

	if(nargs!=2)
	{
		g_print("Error : Wrong number of arguments to g_addweapon\n");
		return 0;
	}

	//Retreive script object from registry
	lua_pushstring(state,G_SCRIPT_KEY);
	lua_gettable(state,LUA_REGISTRYINDEX);
	script=(g_script_t*)lua_touserdata(state,-1);
	world=script->world;

	//Find the weapon in the list
	name=lua_tostring(state,1);
	for(i=0;i<world->numweapons;i++)
	{
		if(!strcmp(world->weapons[i].name,name))
			break;
	}
	if(i>=world->numweapons)
	{
		g_print("Error : Cannot find weapon \"%s\"\n",name);
		return 0;
	}

	//Find the character
	name=lua_tostring(state,2);
	for(j=0;j<world->numcharacters;j++)
	{
		if(!strcmp(world->characters[j].name,name))
			break;
	}
	if(j>=world->numcharacters)
	{
		g_print("Error : Cannot find character \"%s\"\n",name);
		return 0;
	}

	if(world->characters[j].numweapons>=G_MAX_WEAPONS)
	{
		g_print("Error : The character \"%s\" owns too many weapons\n",name);
		return 0;
	}
	world->characters[j].weapons[world->characters[j].numweapons]=i;
	world->characters[j].numweapons++;
	world->characters[j].currentweapon=0;

	return 0;
}

int __g_registercharacter(lua_State* state)
{
	int nargs=lua_gettop(state);
	g_script_t* script;
	g_characterclass_t* type;
	g_world_t* world;

	fs_handle_t file;
	e_model_t* model;
	e_texture_t* texture;
	e_skeleton_t* skeleton;

	if(nargs!=7)
	{
		g_print("Error : Wrong number of arguments to g_registercharacter\n");
		return 0;
	}

	//Retreive script object from registry
	lua_pushstring(state,G_SCRIPT_KEY);
	lua_gettable(state,LUA_REGISTRYINDEX);
	script=(g_script_t*)lua_touserdata(state,-1);
	world=script->world;

	//Try to load character model
	file=fs_open(lua_tostring(state,5),FS_OPEN_READ);
	if(file==FS_INVALID_HANDLE)
	{
		g_print("Error : Cannot open character model file \"%s\"\n",lua_tostring(state,5));
		return 0;
	}
	model=e_loadmodel(script->device,file);
	fs_close(file);
	if(!model)
	{
		g_print("Error : Invalid character model file \"%s\"\n",lua_tostring(state,5));
		return 0;
	}

	//Try to load character skeleton
	file=fs_open(lua_tostring(state,6),FS_OPEN_READ);
	if(file==FS_INVALID_HANDLE)
	{
		e_destroymodel(model);
		g_print("Error : Cannot open character texture file \"%s\"\n",lua_tostring(state,6));
		return 0;
	}
	skeleton=e_loadskeleton(script->device,file);
	fs_close(file);
	if(!skeleton)
	{
		e_destroymodel(model);
		g_print("Error : Invalid character texture file \"%s\"\n",lua_tostring(state,6));
		return 0;
	}

	//Try to load character texture
	file=fs_open(lua_tostring(state,7),FS_OPEN_READ);
	if(file==FS_INVALID_HANDLE)
	{
		e_destroyskeleton(skeleton);
		e_destroymodel(model);
		g_print("Error : Cannot open character texture file \"%s\"\n",lua_tostring(state,7));
		return 0;
	}
	texture=e_loadtexture(script->device,0,file);
	fs_close(file);
	if(!texture)
	{
		e_destroyskeleton(skeleton);
		e_destroymodel(model);
		g_print("Error : Invalid character texture file \"%s\"\n",lua_tostring(state,7));
		return 0;
	}

	world->numcharactertypes++;
	world->charactertypes=sys_realloc(world->charactertypes,sizeof(g_characterclass_t)*world->numcharactertypes);
	type=&world->charactertypes[world->numcharactertypes-1];

	strncpy(type->name,lua_tostring(state,1),G_MAX_CHARACTERNAME-1);
	type->name[G_MAX_CHARACTERNAME-1]='\0';

	type->invmass=(mt_scalar_t)(1.0/lua_tonumber(state,2));
	type->life=(mt_scalar_t)lua_tonumber(state,3);
	type->classid=lua_tointeger(state,4);

	type->model=model;
	type->skeleton=skeleton;
	type->texture=texture;

	return 0;
}

int __g_addcharacter(lua_State* state)
{
	int nargs=lua_gettop(state);
	g_script_t* script;
	g_world_t* world;
	g_character_t* character;
	g_characterclass_t* characterclass;
	const char* type;
	unsigned int i;

	if(nargs!=4)
	{
		g_print("Error : Wrong number of arguments to g_addcharacter\n");
		return 0;
	}

	//Retreive script object from registry
	lua_pushstring(state,G_SCRIPT_KEY);
	lua_gettable(state,LUA_REGISTRYINDEX);
	script=(g_script_t*)lua_touserdata(state,-1);
	world=script->world;

	//We must find the character class
	type=lua_tostring(state,1);
	for(i=0;i<world->numcharactertypes;i++)
	{
		if(!strcmp(type,world->charactertypes[i].name))
			break;
	}
	if(i>=world->numcharactertypes)
	{
		g_print("Error : The given character class \"%s\" has not been registered yet\n",type);
		return 0;
	}

	characterclass=&world->charactertypes[i];

	world->numcharacters++;
	world->characters=(g_character_t*)sys_realloc(world->characters,sizeof(g_character_t)*world->numcharacters);
	character=&world->characters[world->numcharacters-1];

	//Setup character properties
	memset(character,0,sizeof(g_character_t));

	strncpy(character->name,lua_tostring(state,2),G_MAX_CHARACTERNAME-1);
	character->name[G_MAX_CHARACTERNAME-1]='\0';

	mt_mat3id(character->transform.rotation);
	character->pitch=-MT_PI/(mt_scalar_t)4.0;
	character->heading=-MT_PI/(mt_scalar_t)2.0;
	G_READTABLE(state,3,character->transform.position,3);

	character->classid=characterclass->classid;
	character->invmass=characterclass->invmass;
	character->life=characterclass->life;

	character->model=characterclass->model;
	character->modelbuffer=e_createmodelbuffer(character->model);
	character->skeleton=characterclass->skeleton;
	character->skeletonbuffer=e_createskeletonbuffer(character->skeleton);
	character->texture=characterclass->texture;

	character->currentweapon=-1;

	mt_vec3copy(character->halfextents,character->model->halfextents);

	//Setup a default animation state
	character->anim.blend=0;
	character->anim.factor1=(mt_scalar_t)0.0;
	character->anim.action1=G_CHARACTER_WAITING;
	character->anim.anim1[0]=character->skeleton->actions[G_CHARACTER_WAITING].begin;
	character->anim.anim1[1]=character->anim.anim1[0]+1;

	//Here we create a controller for the character
	character->controller=0;
	type=lua_tostring(state,4);
	if(!strcmp(type,"user"))
	{
		g_controller_t* controller=g_createcontroller(G_CONTROLLER_USER);
		
		controller->user.device=script->device;
		controller->user.sensitivity=(mt_scalar_t)0.005;
		controller->user.controls[G_CONTROL_TURNLEFT]=E_KEY_LEFT;
		controller->user.controls[G_CONTROL_TURNRIGHT]=E_KEY_RIGHT;
		controller->user.controls[G_CONTROL_STRAFELEFT]=E_KEY_A;
		controller->user.controls[G_CONTROL_STRAFERIGHT]=E_KEY_D;
		controller->user.controls[G_CONTROL_FORWARD]=E_KEY_W;
		controller->user.controls[G_CONTROL_BACKWARD]=E_KEY_S;
		controller->user.controls[G_CONTROL_JUMP]=E_KEY_SPACE;

		character->controller=controller;
	}
	else
	{
		if(strcmp(type,"ai"))
			g_print("Warning : Invalid controller type in g_addcharacter, using ai controller\n");
		character->controller=g_createcontroller(G_CONTROLLER_AI);
		mt_vec3zero(character->controller->ai.location);
	}

	return 0;
}

int __g_removecharacter(lua_State* state)
{
	int nargs=lua_gettop(state);
	const char* name;
	g_script_t* script;
	g_world_t* world;
	unsigned int i,j;
	
	if(nargs!=1)
	{
		g_print("Error : Wrong number of arguments to g_removecharacter\n");
		return 0;
	}

	//Retreive script object from registry
	lua_pushstring(state,G_SCRIPT_KEY);
	lua_gettable(state,LUA_REGISTRYINDEX);
	script=(g_script_t*)lua_touserdata(state,-1);
	world=script->world;

	//Try to find the character with the corresponding name
	name=lua_tostring(state,1);
	for(i=0;i<world->numcharacters;i++)
	{
		if(!strcmp(world->characters[i].name,name))
			break;
	}
	if(i>=world->numcharacters)
	{
		g_print("Error : There\'s no character called %s\n",name);
		return 0;
	}

	//Don't forget to destroy this character
	g_destroycharacter(&world->characters[i]);

	//Move memory
	for(j=i+1;j<world->numcharacters;j++)
		memcpy(&world->characters[j-1],&world->characters[j],sizeof(g_character_t));
	
	world->numcharacters--;
	world->characters=(g_character_t*)sys_realloc(world->characters,sizeof(g_character_t)*world->numcharacters);
	
	return 0;
}

int __g_lookat(lua_State* state)
{
	int nargs=lua_gettop(state);
	g_script_t* script;
	g_world_t* world;

	lua_pushstring(state,G_SCRIPT_KEY);
	lua_gettable(state,LUA_REGISTRYINDEX);
	script=(g_script_t*)lua_touserdata(state,-1);
	world=script->world;

	if(nargs!=2)
	{
		g_print("Error : Wrong number of arguments to g_lookat\n");
		return 0;
	}

	if(lua_isstring(state,1))
	{
		unsigned int i;
		const char* name;

		name=lua_tostring(state,1);
		for(i=0;i<world->numcharacters;i++)
		{
			if(!strcmp(world->characters[i].name,name))
				break;
		}
		if(i>=world->numcharacters)
		{
			g_print("Error : There\'s no character called %s\n",name);
			return 0;
		}

		g_lookat=i;
		g_distance=(mt_scalar_t)lua_tonumber(state,2);
		return 0;
	}

	if(lua_istable(state,1))
	{
		g_lookat=-1;
		G_READTABLE(state,1,g_camera[0],3);
		G_READTABLE(state,2,g_camera[1],3);
		return 0;
	}

	g_print("Error : Invalid argument type in g_lookat\n");
	return 0;
}

int __g_physics(lua_State* state)
{
	int nargs=lua_gettop(state);

	if(nargs!=5)
	{
		g_print("Error : Wrong number of arguments to g_physics\n");
		return 0;
	}

	g_gravity=(mt_scalar_t)lua_tonumber(state,1);
	g_friction=(mt_scalar_t)lua_tonumber(state,2);
	g_airfriction=(mt_scalar_t)lua_tonumber(state,3);
	g_step=(mt_scalar_t)lua_tonumber(state,4);
	g_jump=(mt_scalar_t)lua_tonumber(state,5);

	return 0;
}

int __g_print(lua_State* state)
{
	int i,nargs=lua_gettop(state);

	for(i=1;i<=nargs;i++)
		g_print(lua_tostring(state,i));

	return 0;
}
