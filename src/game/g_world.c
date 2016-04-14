#include "sys_common.h"
#include "g_console.h"
#include "g_world.h"
#include "g_math.h"
#include "g_variables.h"

g_world_t* g_loadworld(e_device_t* device,const char* path)
{
	static const char gradient[32]={0,0,0,0,0,0,0,0,64,64,64,64,64,64,128,128,
		128,128,128,128,192,192,192,192,192,192,255,255,255,255,255,255};
	unsigned int i,depth;
	fs_handle_t file;
	g_world_t* world;
	mt_vector3_t min,max,extents;

	//Setup working directory
	world=(g_world_t*)sys_alloc(sizeof(g_world_t));
	memset(world,0,sizeof(g_world_t));
	file=fs_open(fs_pathtofile(path,G_MAP_FILENAME),FS_OPEN_READ);
	if(file==FS_INVALID_HANDLE)
	{
		g_log(G_LOG_ERROR,"Cannot load map \"%s\", file is missing",G_MAP_FILENAME);
		return 0;
	}

	world->map=e_loadmap(device,file);
	fs_close(file);
	if(!world->map)
	{
		g_log(G_LOG_ERROR,"Cannot load map \"%s\", invalid format",G_MAP_FILENAME);
		return 0;
	}

	world->numtextures=world->map->numtextures;
	world->textures=(e_texture_t**)sys_alloc(sizeof(e_texture_t*)*world->numtextures);
	g_log(G_LOG_MESSAGE,"Map has %d textures",world->numtextures);

	//We load each texture
	for(i=0;i<world->numtextures;i++)
	{
		file=fs_open(fs_pathtofile(path,world->map->textures[i]),FS_OPEN_READ);
		if(file==FS_INVALID_HANDLE)
		{
			g_log(G_LOG_ERROR,"Cannot load texture \"%s\", file is missing",world->map->textures[i]);
			return 0;
		}

		world->textures[i]=e_loadtexture(device,1,file);
		fs_close(file);
		if(!world->textures[i])
		{
			g_log(G_LOG_ERROR,"Error : Cannot load texture \"%s\", invalid format",world->map->textures[i]);
			return 0;
		}
	}

	//Setup the game gradient (for cel-shading purposes)
	world->gradient=e_creategradient(device,gradient,32);
	
	//We finally create the mesh
	world->mesh=mt_createmesh(world->map->geomindices,world->map->numgeomindices,world->map->geomvertices);
	g_log(G_LOG_MESSAGE,"Mesh depth is %d",world->mesh->depth);

	//Compute the size of the world
	mt_vec3copy(min,world->map->min);
	mt_vec3copy(max,world->map->max);
	mt_vec3diff(max,min,extents);

	//Make the world "bigger" (in order to avoid some problems when a character reaches the boundaries)
	max[0]+=G_WORLD_MARGIN;
	max[1]+=G_WORLD_MARGIN;
	max[2]+=G_WORLD_MARGIN;

	min[0]-=G_WORLD_MARGIN;
	min[1]-=G_WORLD_MARGIN;
	min[2]-=G_WORLD_MARGIN;

	//Setup the divider structure (which is simply an octree)
	depth=(unsigned int)(mt_log(mt_vec3len(extents))/MT_LN8);
	world->divider=mt_createdivider(depth,min,max);
	g_log(G_LOG_MESSAGE,"Divider depth is %d",depth);

	return world;
}

void g_updateworld(g_world_t* world,mt_scalar_t step)
{
	unsigned int i;
	const mt_scalar_t dt=(mt_scalar_t)step;

	//Resize the divider if needed (this is somewhat slow, so we do it only when needed)
	if(world->divider->numcolliders<world->numcharacters)
		mt_resizedivider(world->divider,world->numcharacters);
	
	//Update characters and associated colliders
	for(i=0;i<world->numcharacters;i++)
	{
		g_character_t* character=&world->characters[i];
		g_controller_t* controller=character->controller;
		mt_collider_t* collider=&world->divider->colliders[i];
		mt_vector3_t min,max;

		//We call the character controller to get a new velocity
		controller->func(character,dt);
		mt_mat3yrot(character->transform.rotation,-character->heading);
		
		collider->user=(void*)character;

		//Compute the bounding box of the character
		mt_vec3diff(character->transform.position,character->halfextents,min);
		mt_vec3sum(character->transform.position,character->halfextents,max);
		mt_updatecollider(world->divider,i,min,max);
	}

	for(i=0;i<world->numcharacters;i++)
	{
		g_character_t* character=&world->characters[i];

		//Move the character
		g_charactermove(character,i,world->divider,world->mesh,dt);

		//Let's apply some friction and gravity
		character->velocity[0]-=dt*(character->velocity[0]*g_airfriction);
		character->velocity[1]-=dt*(character->velocity[1]*g_airfriction+g_gravity*character->invmass);
		character->velocity[2]-=dt*(character->velocity[2]*g_airfriction);
		//Apply those forces to the character
		g_charactermove(character,i,world->divider,world->mesh,dt);

		//Update the inverse transformation
		mt_inversetransform(&character->inverse,&character->transform);

		//Set the character flags
		g_characterflags(character,i,world->divider,world->mesh);

		//Add some more friction when the character is on the floor
		if(character->flags&G_CHARACTER_CANJUMP)
			mt_vec3scale(character->velocity,(mt_scalar_t)1.0-g_friction*dt);

		//And animate it
		g_animatecharacter(character,dt);
	}
}

void g_renderworld(e_device_t* device,const g_world_t* world,const e_camera_t* camera)
{
	const mt_matrix4_t rotateleft={0,0,-1,0,
					0,1,0,0,
					1,0,0,0,
					0,0,0,1};
	const mt_matrix4_t rotateright={0,0,1,0,
					0,1,0,0,
					-1,0,0,0,
					0,0,0,1};
	const unsigned int* indices;
	unsigned int count;
	unsigned int i,j;

	//Same thing : same reason (see the code in g_updateworld)
	if(world->divider->numcolliders<world->numcharacters)
		mt_resizedivider(world->divider,world->numcharacters);

	//Get visible characters
	indices=mt_frustumquery(world->divider,&count,camera->frustum);

	//Update their skeleton buffers and model buffers
	for(i=0;i<count;i++)
	{
		g_character_t* character=&world->characters[indices[i]];
		g_rendercharacter_t* render=&character->render;
		mt_matrix4_t handmatrix,matrix;

		//Update skeleton buffer
		e_updateskeletonbuffer(character->skeletonbuffer,character->skeleton,&character->anim);
		
		//Compute the weapon-space matrix
		mt_getmatrix(render->matrix,&character->transform);
		e_getbonematrix(matrix,character->skeleton->hands[1],character->skeleton,character->skeletonbuffer);
		//Rotate the weapon model to left or right, depending on the chosen hand
		mt_mat4rmulmat4(handmatrix,matrix,rotateright);
		mt_mat4rmulmat4(render->weaponmatrix,render->matrix,handmatrix);
		mt_mat4orthoinv(render->invweaponmatrix,render->weaponmatrix);

		//Update the vertex buffers in the model buffer
		e_updatemodelbuffer(character->modelbuffer,character->model,character->skeletonbuffer);
	}
	
	//1st pass : fill the depth buffer
	e_beginpass(device,E_RENDER_MESHES);
	for(j=0;j<count;j++)
	{
		g_character_t* character=&world->characters[indices[j]];
		g_rendercharacter_t* render=&character->render;

		//Render the model mesh (we don't want shading so the "shading" parameter is set to 0)
		e_rendermodel(device,character->model,character->modelbuffer,character->texture,world->gradient,0,render->matrix,0);

		//Render the weapon mesh (the same)
		if(character->currentweapon>=0)
		{
			g_weapon_t* weapon=&world->weapons[character->weapons[character->currentweapon]];
			e_rendermodel(device,weapon->model,weapon->buffer,weapon->texture,world->gradient,0,render->weaponmatrix,0);
		}
	}
	e_rendermap(device,world->map,camera,world->textures,world->gradient,0,0);
	e_endpass(device,E_RENDER_MESHES);

	//A render pass for each light ! (slow, but quite good result)
	for(i=0;i<world->map->numlights;i++)
	{
		//Build shadow volumes if needed
		for(j=0;j<count;j++)
		{
			g_character_t* character=&world->characters[indices[j]];
			g_rendercharacter_t* render=&character->render;
			mt_scalar_t t0=(mt_scalar_t)1.0;
			mt_vector3_t dir;

			//Determine if this light can affect the character's model
			mt_vec3diff(character->transform.position,world->map->lights[i].position,dir);
			if(mt_raytomesh(world->mesh,world->map->lights[i].position,dir,&t0))
			{
				render->islightened=0;
				continue;
			}

			render->islightened=1;

			//Transform the light position to character-space and weapon space
			mt_transformvector(render->light,world->map->lights[i].position,&character->inverse);
			mt_mat4mulvec3(render->weaponlight,render->invweaponmatrix,world->map->lights[i].position);

			//Build shadow volume for this character
			e_buildshadowvolume(character->modelbuffer,character->model,render->light);
		}

		//Clear shadows
		e_clearshadows(device);

		//Render shadow volumes
		e_beginpass(device,E_RENDER_SHADOWS);
		for(j=0;j<count;j++)
		{
			g_character_t* character=&world->characters[indices[j]];
			g_rendercharacter_t* render=&character->render;

			//Skip this character if it is not affected by the current light
			if(!render->islightened)
				continue;

			//Render its shadow volume
			e_rendermodelshadow(device,character->model,character->modelbuffer,render->light,render->matrix);
			
			//Render weapon shadow volume
			if(character->currentweapon>=0)
			{
				g_weapon_t* weapon=&world->weapons[character->weapons[character->currentweapon]];
				
				e_buildshadowvolume(weapon->buffer,weapon->model,render->weaponlight);
				e_rendermodelshadow(device,weapon->model,weapon->buffer,render->weaponlight,render->weaponmatrix);
			}
		}
		e_endpass(device,E_RENDER_SHADOWS);

		//Render scene with shading
		e_beginpass(device,E_RENDER_SHADING);
		for(j=0;j<count;j++)
		{
			g_character_t* character=&world->characters[indices[j]];
			g_rendercharacter_t* render=&character->render;

			//Skip this character if it is not affected by the current light
			if(!render->islightened)
				continue;

			//Render the shaded model
			e_rendermodel(device,character->model,character->modelbuffer,character->texture,world->gradient,render->light,render->matrix,1);

			//Render the shaded weapon
			if(character->currentweapon>=0)
			{
				g_weapon_t* weapon=&world->weapons[character->weapons[character->currentweapon]];
				e_rendermodel(device,weapon->model,weapon->buffer,weapon->texture,world->gradient,render->weaponlight,render->weaponmatrix,1);
			}
		}
		e_rendermap(device,world->map,camera,world->textures,world->gradient,world->map->lights[i].position,1);
		e_endpass(device,E_RENDER_SHADING);
	}

	//Last pass : draw outlines
	e_beginpass(device,E_RENDER_OUTLINES);
	for(j=0;j<count;j++)
	{
		g_character_t* character=&world->characters[indices[j]];
		g_rendercharacter_t* render=&character->render;

		//Render the shaded model
		e_rendermodeloutlines(device,character->model,character->modelbuffer,render->matrix);
			
		//Render the shaded weapon
		if(character->currentweapon>=0)
		{
			g_weapon_t* weapon=&world->weapons[character->weapons[character->currentweapon]];
			e_rendermodeloutlines(device,weapon->model,weapon->buffer,render->weaponmatrix);
		}
	}
	//e_rendermapoutlines(device,world->map,camera);
	e_endpass(device,E_RENDER_OUTLINES);
}

void g_destroyworld(g_world_t* world)
{
	unsigned int i;

	if(world->map)
		e_destroymap(world->map);
	if(world->textures)
	{
		for(i=0;i<world->numtextures;i++)
		{
			if(world->textures[i])
				e_destroytexture(world->textures[i]);
		}
		sys_free(world->textures);
	}

	if(world->gradient)
		e_destroytexture(world->gradient);

	if(world->mesh)
		mt_destroymesh(world->mesh);
	if(world->divider)
		mt_destroydivider(world->divider);

	sys_free(world);
}
