#include "e_model.h"
#include "e_programs.h"

const e_filetype_t e_modelfile={('M'+('D'<<8)+('L'<<16)+('1'<<24)),1};

e_modelbuffer_t* e_createmodelbuffer(e_model_t* model)
{
	e_modelbuffer_t* buffer=sys_alloc(sizeof(e_modelbuffer_t));
	const unsigned int numtriangles=model->numindices/3;	
	unsigned int i,j;

	glGenBuffersARB(1,&buffer->modelbuffer);
	glGenBuffersARB(1,&buffer->shadowbuffer);
	
	buffer->volume=(unsigned int*)sys_alloc(sizeof(unsigned int)*(2*model->numindices+6*model->numedges));
	buffer->planes=(e_vertex4_t*)sys_alloc(sizeof(e_vertex4_t)*numtriangles);
	buffer->facing=(unsigned char*)sys_alloc(sizeof(unsigned char)*(numtriangles+1));

	//For static models, we fill the buffers at once
	if(model->flags&E_MODEL_STATIC)
	{
		mt_vector3_t* vertices;
		e_vertex4_t* shadow;

		//This is a static model
		glBindBufferARB(GL_ARRAY_BUFFER_ARB,buffer->modelbuffer);
		glBufferDataARB(GL_ARRAY_BUFFER_ARB,sizeof(e_vertex3_t)*2*model->numvertices,0,GL_STATIC_DRAW_ARB);
		vertices=(mt_vector3_t*)glMapBufferARB(GL_ARRAY_BUFFER_ARB,GL_WRITE_ONLY_ARB);

		glBindBufferARB(GL_ARRAY_BUFFER_ARB,buffer->shadowbuffer);
		glBufferDataARB(GL_ARRAY_BUFFER_ARB,sizeof(e_vertex4_t)*2*model->numvertices,0,GL_STATIC_DRAW_ARB);
		shadow=(e_vertex4_t*)glMapBufferARB(GL_ARRAY_BUFFER_ARB,GL_WRITE_ONLY_ARB);

		//Send data to static buffers
		for(i=0,j=0;i<model->numvertices;i++,j+=2)
		{
			vertices[j][0]=model->vertices[i].pos[0];
			vertices[j][1]=model->vertices[i].pos[1];
			vertices[j][2]=model->vertices[i].pos[2];

			vertices[j+1][0]=model->vertices[i].normal[0];
			vertices[j+1][1]=model->vertices[i].normal[1];
			vertices[j+1][2]=model->vertices[i].normal[2];

			shadow[j][0]=model->vertices[i].pos[0];
			shadow[j][1]=model->vertices[i].pos[1];
			shadow[j][2]=model->vertices[i].pos[2];
			shadow[j][3]=(mt_scalar_t)1.0;

			shadow[j+1][0]=model->vertices[i].pos[0];
			shadow[j+1][1]=model->vertices[i].pos[1];
			shadow[j+1][2]=model->vertices[i].pos[2];
			shadow[j+1][3]=(mt_scalar_t)0.0;
		}

		glBindBufferARB(GL_ARRAY_BUFFER_ARB,buffer->shadowbuffer);
		glUnmapBufferARB(GL_ARRAY_BUFFER_ARB);
		glBindBufferARB(GL_ARRAY_BUFFER_ARB,buffer->modelbuffer);
		glUnmapBufferARB(GL_ARRAY_BUFFER_ARB);

		//Compute plane equations
		for(i=0;i<model->numindices;i+=3)
		{
			const unsigned int p=model->indices[i];
			const unsigned int q=model->indices[i+1];
			const unsigned int r=model->indices[i+2];
			mt_scalar_t* plane=buffer->planes[i/3];

			mt_triangleplane(model->vertices[p].pos,model->vertices[q].pos,model->vertices[r].pos,plane);
		}
	}

	return buffer;
}

void e_updatemodelbuffer(e_modelbuffer_t* buffer,const e_model_t* model,const e_skeletonbuffer_t* skeleton)
{
	unsigned int i,j,k;
	e_vertex3_t* vertices;
	e_vertex4_t* shadow;
	mt_vector3_t v0,v1,n0,n1;

	//These computations are only necesary when the model is dynamic
	if(!(model->flags&E_MODEL_STATIC))
	{
		glBindBufferARB(GL_ARRAY_BUFFER_ARB,buffer->modelbuffer);
		glBufferDataARB(GL_ARRAY_BUFFER_ARB,sizeof(e_vertex3_t)*2*model->numvertices,0,GL_DYNAMIC_DRAW_ARB);
		vertices=(e_vertex3_t*)glMapBufferARB(GL_ARRAY_BUFFER_ARB,GL_WRITE_ONLY_ARB);

		glBindBufferARB(GL_ARRAY_BUFFER_ARB,buffer->shadowbuffer);
		glBufferDataARB(GL_ARRAY_BUFFER_ARB,sizeof(e_vertex4_t)*2*model->numvertices,0,GL_DYNAMIC_DRAW_ARB);
		shadow=(e_vertex4_t*)glMapBufferARB(GL_ARRAY_BUFFER_ARB,GL_WRITE_ONLY_ARB);

		//Transform the vertices using the skeleton information
		for(i=0,j=0;i<model->numvertices;i++,j+=2)
		{
			const e_weight_t* weight=&model->weights[i];

			v0[0]=n0[0]=0;
			v0[1]=n0[1]=0;
			v0[2]=n0[2]=0;

			//Transform the vertices and normals
			for(k=0;k<E_MODEL_MAXBONES&&weight->bones[k]>=0;k++)
			{
				mt_mat4mulvec3(v1,skeleton->current[weight->bones[k]],model->vertices[i].pos);
				mt_mat4rmulvec3(n1,skeleton->current[weight->bones[k]],model->vertices[i].normal);

				v0[0]+=v1[0]*weight->weights[k];
				v0[1]+=v1[1]*weight->weights[k];
				v0[2]+=v1[2]*weight->weights[k];

				n0[0]+=n1[0]*weight->weights[k];
				n0[1]+=n1[1]*weight->weights[k];
				n0[2]+=n1[2]*weight->weights[k];
			}

			//Copy the computed vertices and normals to the vertex buffer
			vertices[j][0]=v0[0];
			vertices[j][1]=v0[1];
			vertices[j][2]=v0[2];

			vertices[j+1][0]=n0[0];
			vertices[j+1][1]=n0[1];
			vertices[j+1][2]=n0[2];

			//Copy the computed vertices to the shadow buffer
			shadow[j][0]=v0[0];
			shadow[j][1]=v0[1];
			shadow[j][2]=v0[2];
			shadow[j][3]=(mt_scalar_t)1.0;

			shadow[j+1][0]=v0[0];
			shadow[j+1][1]=v0[1];
			shadow[j+1][2]=v0[2];
			shadow[j+1][3]=(mt_scalar_t)0.0;

			//Copy the computed vertices to the CPU memory
			model->vertexbuffer[i][0]=v0[0];
			model->vertexbuffer[i][1]=v0[1];
			model->vertexbuffer[i][2]=v0[2];
		}

		glBindBufferARB(GL_ARRAY_BUFFER_ARB,buffer->shadowbuffer);
		glUnmapBufferARB(GL_ARRAY_BUFFER_ARB);
		glBindBufferARB(GL_ARRAY_BUFFER_ARB,buffer->modelbuffer);
		glUnmapBufferARB(GL_ARRAY_BUFFER_ARB);

		//Compute new plane equations
		for(i=0;i<model->numindices;i+=3)
		{
			const unsigned int p=model->indices[i];
			const unsigned int q=model->indices[i+1];
			const unsigned int r=model->indices[i+2];
			mt_scalar_t* plane=buffer->planes[i/3];

			mt_triangleplane(model->vertexbuffer[p],model->vertexbuffer[q],model->vertexbuffer[r],plane);
		}
	}
}

void e_buildshadowvolume(e_modelbuffer_t* buffer,const e_model_t* model,const mt_vector3_t light)
{
	const unsigned int numtriangles=model->numindices/3;
	unsigned int j;

	//Compute the "facing" array
	for(j=0;j<numtriangles;j++)
		buffer->facing[j]=(mt_planedistance(buffer->planes[j],light)>(mt_scalar_t)0.0);
	buffer->facing[numtriangles]=1;

	//Build shadow volume
	buffer->count=e_createsilhouette(buffer->volume,buffer->facing,model->edges,model->numedges);
	buffer->count+=e_createcaps(buffer->volume+buffer->count,buffer->facing,model->indices,model->numindices);
}

void e_destroymodelbuffer(e_modelbuffer_t* buffer)
{
	glDeleteBuffersARB(1,&buffer->modelbuffer);
	glDeleteBuffersARB(1,&buffer->shadowbuffer);

	sys_free(buffer->volume);
	sys_free(buffer->planes);
	sys_free(buffer->facing);
	sys_free(buffer);
}

e_model_t* e_loadmodel(e_device_t* device,fs_handle_t file)
{
	e_filetype_t header;
	e_texcoord_t* texcoords;
	e_model_t* model;
	unsigned int i;

	//Check header
	fs_read(&header,sizeof(e_filetype_t),file);
	if(!e_checkfiletype(&header,&e_modelfile))
		return 0;

	model=(e_model_t*)sys_alloc(sizeof(e_model_t));
	memset(model,0,sizeof(e_model_t));

	//Read model info
	fs_read(&model->numvertices,sizeof(unsigned int),file);
	fs_read(&model->numindices,sizeof(unsigned int),file);
	fs_read(&model->numedges,sizeof(unsigned int),file);
	fs_read(&model->flags,sizeof(int),file);
	fs_read(model->halfextents,sizeof(mt_vector3_t),file);

	model->vertices=(e_modelvertex_t*)sys_alloc(sizeof(e_modelvertex_t)*model->numvertices);
	model->indices=(unsigned int*)sys_alloc(sizeof(unsigned int)*model->numindices);
	model->edges=(e_edge_t*)sys_alloc(sizeof(e_edge_t)*model->numedges);

	//Loading procedure depends on the model type
	if(model->flags&E_MODEL_STATIC)
		fs_read(model->vertices,sizeof(e_modelvertex_t)*model->numvertices,file);
	else
	{
		model->weights=(e_weight_t*)sys_alloc(sizeof(e_weight_t)*model->numvertices);
		model->vertexbuffer=(mt_vector3_t*)sys_alloc(sizeof(mt_vector3_t)*model->numvertices);
		for(i=0;i<model->numvertices;i++)
		{
			fs_read(&model->vertices[i],sizeof(e_modelvertex_t),file);
			fs_read(&model->weights[i],sizeof(e_weight_t),file);
		}
	}

	//Read indices
	fs_read(model->indices,sizeof(unsigned int)*model->numindices,file);
	glGenBuffersARB(1,&model->indexbuffer);
	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB,model->indexbuffer);
	glBufferDataARB(GL_ELEMENT_ARRAY_BUFFER_ARB,sizeof(unsigned int)*model->numindices,model->indices,GL_STATIC_DRAW);
	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB,0);

	//Read edges
	fs_read(model->edges,sizeof(e_edge_t)*model->numedges,file);

	//Copy texcoords to buffer
	glGenBuffersARB(1,&model->texcoordbuffer);
	glBindBufferARB(GL_ARRAY_BUFFER_ARB,model->texcoordbuffer);
	glBufferDataARB(GL_ARRAY_BUFFER_ARB,sizeof(e_texcoord_t)*model->numvertices,0,GL_STATIC_DRAW_ARB);
	texcoords=(e_texcoord_t*)glMapBufferARB(GL_ARRAY_BUFFER_ARB,GL_WRITE_ONLY_ARB);
	for(i=0;i<model->numvertices;i++)
	{
		texcoords[i][0]=model->vertices[i].tex[0];
		texcoords[i][1]=model->vertices[i].tex[1];
	}
	glUnmapBufferARB(GL_ARRAY_BUFFER_ARB);
	glBindBufferARB(GL_ARRAY_BUFFER_ARB,0);

	return model;
}

void e_destroymodel(e_model_t* model)
{
	glDeleteBuffersARB(1,&model->texcoordbuffer);
	glDeleteBuffersARB(1,&model->indexbuffer);

	if(!(model->flags&E_MODEL_STATIC))
	{
		sys_free(model->vertexbuffer);
		sys_free(model->weights);
	}

	sys_free(model->edges);
	sys_free(model->vertices);
	sys_free(model->indices);

	sys_free(model);
}

void e_rendermodel(e_device_t* device,e_model_t* model,const e_modelbuffer_t* buffer,const e_texture_t* texture,const e_texture_t* gradient,const mt_vector3_t light,const mt_matrix4_t matrix,int shading)
{
	const float one[4]={1.0f,1.0f,1.0f,1.0f};

	glPushMatrix();
#ifndef E_DOUBLE
	glMultMatrixf(matrix);
#else
	glMultMatrixd(matrix);
#endif

	//Setup vertices
	glBindBufferARB(GL_ARRAY_BUFFER_ARB,buffer->modelbuffer);
#ifdef E_DOUBLE
	glVertexPointer(3,GL_DOUBLE,sizeof(e_vertex3_t)*2,0);
#else
	glVertexPointer(3,GL_FLOAT,sizeof(e_vertex3_t)*2,0);
#endif
	glEnableClientState(GL_VERTEX_ARRAY);

	//Setup normals
#ifdef E_DOUBLE
	glNormalPointer(GL_DOUBLE,sizeof(mt_vector3_t)*2,(char*)sizeof(e_vertex3_t));
#else
	glNormalPointer(GL_FLOAT,sizeof(e_vertex3_t)*2,(char*)sizeof(e_vertex3_t));
#endif
	glEnableClientState(GL_NORMAL_ARRAY);

	//Setup texture
	glBindBufferARB(GL_ARRAY_BUFFER_ARB,model->texcoordbuffer);
#ifdef E_DOUBLE
	glTexCoordPointer(2,GL_DOUBLE,0,0);
#else
	glTexCoordPointer(2,GL_FLOAT,0,0);
#endif
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glBindTexture(GL_TEXTURE_2D,texture->id);
	glEnable(GL_TEXTURE_2D);

	//Setup cel-shading texture
	glActiveTextureARB(GL_TEXTURE1_ARB);
	glBindTexture(GL_TEXTURE_1D,gradient->id);
	glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_MODULATE);
	glEnable(GL_TEXTURE_1D);
	if(shading)
	{
		const float param[4]={(float)light[0],(float)light[1],(float)light[2],0};
			
		//Setup shading program
		glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB,e_programids[E_PROGRAM_CELSHADING]);
		glProgramEnvParameter4fvARB(GL_FRAGMENT_PROGRAM_ARB,0,one);	//Has texture
		glProgramEnvParameter4fvARB(GL_FRAGMENT_PROGRAM_ARB,1,param); //Send light position
	}
	else
	{
		glBindProgramARB(GL_FRAGMENT_PROGRAM_ARB,e_programids[E_PROGRAM_NOSHADING]);
		glProgramEnvParameter4fvARB(GL_FRAGMENT_PROGRAM_ARB,0,one);	//Has texture
	}

	//Setup index buffer
	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB,model->indexbuffer);
	//Draw model
	glDrawElements(GL_TRIANGLES,model->numindices,GL_UNSIGNED_INT,0);

	//Disable shading texture
	glDisable(GL_TEXTURE_1D);

	glClientActiveTextureARB(GL_TEXTURE0_ARB);
	glActiveTextureARB(GL_TEXTURE0_ARB);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisable(GL_TEXTURE_2D);

	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);

	glBindBufferARB(GL_ARRAY_BUFFER_ARB,0);
	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB,0);

	glPopMatrix();
}

void e_rendermodelshadow(e_device_t* device,e_model_t* model,const e_modelbuffer_t* buffer,const mt_vector3_t light,const mt_matrix4_t matrix)
{
	const float param[4]={(float)light[0],(float)light[1],(float)light[2],0.0f};

	glPushMatrix();
#ifndef E_DOUBLE
	glMultMatrixf(matrix);
#else
	glMultMatrixd(matrix);
#endif

	//Shadow volume extrusion is done within a vertex program
	glBindBufferARB(GL_ARRAY_BUFFER_ARB,buffer->shadowbuffer);
#ifndef E_DOUBLE
	glVertexPointer(4,GL_FLOAT,0,0);
#else
	glVertexPointer(4,GL_DOUBLE,0,0);
#endif
	glEnableClientState(GL_VERTEX_ARRAY);

	//Send the position of the light to the vertex program
	glProgramEnvParameter4fvARB(GL_VERTEX_PROGRAM_ARB,0,param);
		
	//If we have a two sided stencil buffer, we can draw our volume only once
	if(glActiveStencilFaceEXT)
		glDrawElements(GL_TRIANGLES,buffer->count,GL_UNSIGNED_INT,buffer->volume);
	else
	{
		//Draw back faces
		glStencilOp(GL_KEEP,GL_INCR,GL_KEEP);
		glCullFace(GL_FRONT);
		glDrawElements(GL_TRIANGLES,buffer->count,GL_UNSIGNED_INT,buffer->volume);

		//Draw front faces
		glStencilOp(GL_KEEP,GL_DECR,GL_KEEP);
		glCullFace(GL_BACK);
		glDrawElements(GL_TRIANGLES,buffer->count,GL_UNSIGNED_INT,buffer->volume);
	}

	glBindBufferARB(GL_ARRAY_BUFFER_ARB,0);
	glDisableClientState(GL_VERTEX_ARRAY);

	glPopMatrix();
}

void e_rendermodeloutlines(e_device_t* device,e_model_t* model,const e_modelbuffer_t* buffer,const mt_matrix4_t matrix)
{
	const float one[4]={1.0f,1.0f,1.0f,1.0f};

	glPushMatrix();
#ifndef E_DOUBLE
	glMultMatrixf(matrix);
#else
	glMultMatrixd(matrix);
#endif

	//Setup vertices
	glBindBufferARB(GL_ARRAY_BUFFER_ARB,buffer->modelbuffer);
#ifdef E_DOUBLE
	glVertexPointer(3,GL_DOUBLE,sizeof(e_vertex3_t)*2,0);
#else
	glVertexPointer(3,GL_FLOAT,sizeof(e_vertex3_t)*2,0);
#endif
	glEnableClientState(GL_VERTEX_ARRAY);

	//Setup index buffer
	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB,model->indexbuffer);
	//Draw model
	glDrawElements(GL_TRIANGLES,model->numindices,GL_UNSIGNED_INT,0);

	glDisableClientState(GL_VERTEX_ARRAY);

	glBindBufferARB(GL_ARRAY_BUFFER_ARB,0);
	glBindBufferARB(GL_ELEMENT_ARRAY_BUFFER_ARB,0);

	glPopMatrix();
}