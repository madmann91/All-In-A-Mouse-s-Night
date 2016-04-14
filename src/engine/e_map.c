#include "config.h"
#include "e_map.h"
#include "e_programs.h"

const e_filetype_t e_mapfile={('M'+('A'<<8)+('P'<<16)+('1'<<24)),1};

struct e_maprender_s
{
	e_device_t* device;
	const e_map_t* map;
	const e_camera_t* camera;
	const e_texture_t** textures;
	int shading;
};

typedef struct e_maprender_s e_maprender_t;

static int e_planetest(const mt_vector3_t min,const mt_vector3_t max,const mt_plane_t plane)
{
	mt_vector3_t v;

	v[0]=(plane[0]<0)?min[0]:max[0];
	v[1]=(plane[1]<0)?min[1]:max[1];
	v[2]=(plane[2]<0)?min[2]:max[2];
	if(plane[0]*v[0]+plane[1]*v[1]+plane[2]*v[2]-plane[3]<0)
		return 0;

	return 1;
}

static void e_renderleaf(const e_mapleaf_t* leaf,const e_maprender_t* render)
{
	unsigned int start=0;
	unsigned int i;

	//Each pass is a couple of (end,material)
	//Where end is the ending vertex index
	//and material the material index
	for(i=0;i<leaf->numpasses;i++)
	{
		const unsigned int* pass=leaf->passes+i*2;
		const e_mapmaterial_t* material=&render->map->materials[pass[1]];
		const float zero[4]={0.0f,0.0f,0.0f,0.0f};
		const float one[4]={1.0f,1.0f,1.0f,1.0f};

		glColor4ubv(material->color);

		//If this material has no shading, we do not draw it... unless we are in "no shading mode"
		if(!(material->flags&E_MATERIAL_SHADING)&&render->shading)
		{
			start=pass[0]+3;
			continue;
		}

		//Enable texturing if needed
		if(material->flags&E_MATERIAL_TEXTURE)
		{
			//Enable main texture
			glActiveTextureARB(GL_TEXTURE0_ARB);
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D,render->textures[material->texture]->id);
			glProgramEnvParameter4fvARB(GL_FRAGMENT_PROGRAM_ARB,0,one);		//Has texture
		}
		else
		{
			//Disable main texture
			glActiveTextureARB(GL_TEXTURE0_ARB);
			glDisable(GL_TEXTURE_2D);
			glProgramEnvParameter4fvARB(GL_FRAGMENT_PROGRAM_ARB,0,zero);	//No texture
		}
		
		glDrawElements(GL_TRIANGLES,pass[0]-start+3,GL_UNSIGNED_INT,&leaf->indices[start]);

		start=pass[0]+3;
	}
}

static void e_rendercell(const e_mapcell_t* cell,const e_maprender_t* render)
{
	if(!cell->isleaf)
	{
		const e_mapnode_t* node=&render->map->nodes[cell->index];
		unsigned int i;

		//Here we test the cell for intersection
		for(i=0;i<6;i++)
		{
			if(!e_planetest(node->min,node->max,render->camera->frustum[i]))
				return;
		}

		//And if needed continue recursion
		e_rendercell(&render->map->cells[cell->left],render);
		e_rendercell(&render->map->cells[cell->right],render);
	}
	else
		e_renderleaf(&render->map->leaves[cell->index],render);
}

e_map_t* e_loadmap(e_device_t* device,fs_handle_t file)
{
	e_filetype_t header;
	unsigned int size;
	unsigned int i;
	e_map_t* map;
	void* data;

	fs_read(&header,sizeof(e_filetype_t),file);
	if(!e_checkfiletype(&header,&e_mapfile))
		return 0;

	map=(e_map_t*)sys_alloc(sizeof(e_map_t));
	fs_read(&map->numtextures,sizeof(unsigned int),file);
	fs_read(&map->nummaterials,sizeof(unsigned int),file);
	fs_read(&map->numlights,sizeof(unsigned int),file);
	fs_read(&map->numgeomvertices,sizeof(unsigned int),file);
	fs_read(&map->numgeomindices,sizeof(unsigned int),file);
	fs_read(&map->numvertices,sizeof(unsigned int),file);
	fs_read(&map->numcells,sizeof(unsigned int),file);
	fs_read(&map->numnodes,sizeof(unsigned int),file);
	fs_read(&map->numleaves,sizeof(unsigned int),file);
	fs_read(map->min,sizeof(mt_scalar_t)*3,file);
	fs_read(map->max,sizeof(mt_scalar_t)*3,file);

	//Read textures names
	map->textures=(char**)sys_alloc(sizeof(char*)*map->numtextures);
	for(i=0;i<map->numtextures;i++)
	{
		unsigned int len;
		fs_read(&len,sizeof(unsigned int),file);
		map->textures[i]=(char*)sys_alloc(sizeof(char)*(len+1));
		fs_read(map->textures[i],sizeof(char)*len,file);
		map->textures[i][len]=0;
	}

	//Read materials
	size=sizeof(e_mapmaterial_t)*map->nummaterials;
	map->materials=(e_mapmaterial_t*)sys_alloc(size);
	fs_read(map->materials,size,file);

	//Read lights
	size=sizeof(e_maplight_t)*map->numlights;
	map->lights=(e_maplight_t*)sys_alloc(size);
	fs_read(map->lights,size,file);

	//Read geometry vertices
	size=sizeof(mt_scalar_t)*3*map->numgeomvertices;
	map->geomvertices=(mt_scalar_t*)sys_alloc(size);
	fs_read(map->geomvertices,size,file);

	//Read geometry indices
	size=sizeof(unsigned int)*map->numgeomindices;
	map->geomindices=(unsigned int*)sys_alloc(size);
	fs_read(map->geomindices,size,file);

	//Read map vertices
	size=sizeof(e_mapvertex_t)*map->numvertices;
	glGenBuffersARB(1,&map->vertexbuffer);
	glBindBufferARB(GL_ARRAY_BUFFER_ARB,map->vertexbuffer);
	glBufferDataARB(GL_ARRAY_BUFFER_ARB,size,0,GL_STATIC_DRAW);
	data=glMapBufferARB(GL_ARRAY_BUFFER_ARB,GL_WRITE_ONLY);
	fs_read(data,size,file);
	glUnmapBufferARB(GL_ARRAY_BUFFER_ARB);
	glBindBufferARB(GL_ARRAY_BUFFER_ARB,0);
	
	//Here we read the stored AABB tree
	//Read cells
	map->cells=(e_mapcell_t*)sys_alloc(sizeof(e_mapcell_t)*map->numcells);
	for(i=0;i<map->numcells;i++)
	{
		fs_read(&map->cells[i].isleaf,sizeof(unsigned char),file);
		fs_read(&map->cells[i].index,sizeof(unsigned int),file);
		if(!map->cells[i].isleaf)
		{
			fs_read(&map->cells[i].left,sizeof(unsigned int),file);
			fs_read(&map->cells[i].right,sizeof(unsigned int),file);
		}
	}

	//Read nodes
	map->nodes=(e_mapnode_t*)sys_alloc(sizeof(e_mapnode_t)*map->numnodes);
	for(i=0;i<map->numnodes;i++)
	{
		e_mapnode_t* node=&map->nodes[i];
		fs_read(&node->axis,sizeof(unsigned char),file);
		fs_read(node->min,sizeof(mt_scalar_t)*3,file);
		fs_read(node->max,sizeof(mt_scalar_t)*3,file);
		fs_read(&node->split,sizeof(mt_scalar_t),file);
	}

	//Read leaves
	map->leaves=(e_mapleaf_t*)sys_alloc(sizeof(e_mapleaf_t)*map->numleaves);
	for(i=0;i<map->numleaves;i++)
	{
		e_mapleaf_t* leaf=&map->leaves[i];

		fs_read(&leaf->numpasses,sizeof(unsigned int),file);
		size=sizeof(unsigned int)*leaf->numpasses*2;
		leaf->passes=(unsigned int*)sys_alloc(size);
		fs_read(leaf->passes,size,file);

		fs_read(&leaf->numindices,sizeof(unsigned int),file);
		size=sizeof(unsigned int)*leaf->numindices;
		leaf->indices=(unsigned int*)sys_alloc(size);
		fs_read(leaf->indices,size,file);
	}

	return map;
}

void e_destroymap(e_map_t* map)
{
	unsigned int i;
	glDeleteBuffersARB(1,&map->vertexbuffer);
	for(i=0;i<map->numtextures;i++)
		sys_free(map->textures[i]);
	sys_free(map->textures);
	sys_free(map->materials);
	sys_free(map->lights);
	sys_free(map->geomvertices);
	sys_free(map->geomindices);
	sys_free(map->cells);
	sys_free(map->nodes);
	for(i=0;i<map->numleaves;i++)
	{
		sys_free(map->leaves[i].passes);
		sys_free(map->leaves[i].indices);
	}
	sys_free(map->leaves);
	sys_free(map);
}

void e_rendermap(e_device_t* device,const e_map_t* map,const e_camera_t* camera,const e_texture_t** textures,const e_texture_t* gradient,const mt_vector3_t light,int shading)
{
	const e_maprender_t render={device,map,camera,textures,shading};
	const float zero[4]={0.0f,0.0f,0.0f,0.0f};
	const float one[4]={1.0f,1.0f,1.0f,1.0f};

	//Setup vertex buffer
	glBindBufferARB(GL_ARRAY_BUFFER_ARB,map->vertexbuffer);
#ifdef E_DOUBLE
	glVertexPointer(3,GL_DOUBLE,sizeof(e_mapvertex_t),0);
#else
	glVertexPointer(3,GL_FLOAT,sizeof(e_mapvertex_t),0);
#endif
	glEnableClientState(GL_VERTEX_ARRAY);

	//Setub vertex normals buffer
#ifdef E_DOUBLE
	glNormalPointer(GL_DOUBLE,sizeof(e_mapvertex_t),(char*)sizeof(e_vertex3_t));
#else
	glNormalPointer(GL_FLOAT,sizeof(e_mapvertex_t),(char*)sizeof(e_vertex3_t));
#endif
	glEnableClientState(GL_NORMAL_ARRAY);
	
	//Setup shading texture unit
	glClientActiveTextureARB(GL_TEXTURE1_ARB);
	glActiveTextureARB(GL_TEXTURE1_ARB);
	glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
	glBindTexture(GL_TEXTURE_1D,gradient->id);
	glEnable(GL_TEXTURE_1D);
	if(shading)
	{
		const float param[4]={(float)light[0],(float)light[1],(float)light[2],0};

		//Enable shading
		glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB,e_programids[E_PROGRAM_CELSHADING]);
		glProgramEnvParameter4fvARB(GL_FRAGMENT_PROGRAM_ARB,1,param);
	}
	else
	{
		//Disable shading
		glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB,e_programids[E_PROGRAM_NOSHADING]);
		glProgramEnvParameter4fvARB(GL_FRAGMENT_PROGRAM_ARB,1,zero);	//Shading texture coordinate
	}
	
	//Setup main texture unit
	glClientActiveTextureARB(GL_TEXTURE0_ARB);
	glActiveTextureARB(GL_TEXTURE0_ARB);
#ifdef E_DOUBLE
	glTexCoordPointer(2,GL_DOUBLE,sizeof(e_mapvertex_t),(char*)(2*sizeof(e_vertex3_t)));
#else
	glTexCoordPointer(2,GL_FLOAT,sizeof(e_mapvertex_t),(char*)(2*sizeof(e_vertex3_t)));
#endif
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glEnable(GL_TEXTURE_2D);

	glCullFace(GL_BACK);

	//Recursively render the map
	e_rendercell(&map->cells[0],&render);

	glActiveTextureARB(GL_TEXTURE1_ARB);
	glDisable(GL_TEXTURE_1D);

	glClientActiveTextureARB(GL_TEXTURE0_ARB);
	glActiveTextureARB(GL_TEXTURE0_ARB);
	glDisable(GL_TEXTURE_2D);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);

	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	glBindBufferARB(GL_ARRAY_BUFFER_ARB,0);

	glColor3f(1.0f,1.0f,1.0f);
}

