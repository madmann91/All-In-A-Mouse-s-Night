#include "config.h"
#include "e_shadow.h"
#include "e_programs.h"

unsigned int e_createcaps(unsigned int* volume,const unsigned char* facing,const unsigned int* indices,unsigned int numindices)
{
	unsigned int i;
	unsigned int* current;

	current=volume;
	for(i=0;i<numindices;i+=3)
	{
		const unsigned int i0=indices[i]*2;
		const unsigned int i1=indices[i+1]*2;
		const unsigned int i2=indices[i+2]*2;

		if(facing[i/3])
			continue;
		
		current[0]=i2;
		current[1]=i1;
		current[2]=i0;
		current[3]=i0+1;
		current[4]=i1+1;
		current[5]=i2+1;
		current+=6;
	}

	return current-volume;
}

unsigned int e_createsilhouette(unsigned int* volume,const unsigned char* facing,const e_edge_t* edges,unsigned int numedges)
{
	unsigned int i;
	unsigned int* current;

	current=volume;
	for(i=0;i<numedges;i++)
	{
		const e_edge_t* edge=&edges[i];
		const unsigned char f0=facing[edge->t0];
		const unsigned char f1=facing[edge->t1];

		if(f0!=f1) 
		{ 
			const unsigned int v0=edge->v0*2;
			const unsigned int v1=edge->v1*2;

			if(f0)
			{
				current[0]=v0;
				current[1]=v1+1;
				current[2]=v1;
				current[3]=v0;
				current[4]=v0+1;
				current[5]=v1+1;
			}
			else 
			{
				current[0]=v0;
				current[1]=v1;
				current[2]=v1+1;
				current[3]=v0+1;
				current[4]=v0;
				current[5]=v1+1;
			}

			current+=6;
		}
	}

	return current-volume;
}

void e_clearshadows(e_device_t* device)
{
	glClear(GL_STENCIL_BUFFER_BIT);
}

void e_beginpass(e_device_t* device,unsigned int pass)
{
	glPushAttrib(GL_ENABLE_BIT|GL_STENCIL_BUFFER_BIT|GL_POLYGON_BIT);
	switch(pass)
	{
		case E_RENDER_MESHES:
			glBindProgramARB(GL_VERTEX_PROGRAM_ARB,e_programids[E_PROGRAM_PASSTOFRAGMENT]);
			glEnable(GL_VERTEX_PROGRAM_ARB);
			glEnable(GL_FRAGMENT_PROGRAM_ARB);

			glEnable(GL_DEPTH_TEST);
			glEnable(GL_CULL_FACE);
			break;
		case E_RENDER_SHADOWS:
			//The support of this extension depends on the 3D card
			if(glActiveStencilFaceEXT)
			{
				glActiveStencilFaceEXT(GL_BACK);
				glStencilOp(GL_KEEP,GL_INCR_WRAP_EXT,GL_KEEP);
				glActiveStencilFaceEXT(GL_FRONT);
				glStencilOp(GL_KEEP,GL_DECR_WRAP_EXT,GL_KEEP);
				glEnable(GL_STENCIL_TEST_TWO_SIDE_EXT);
			}
			else
				glEnable(GL_CULL_FACE);

			glBindProgramARB(GL_VERTEX_PROGRAM_ARB,e_programids[E_PROGRAM_SHADOW]);
			glEnable(GL_VERTEX_PROGRAM_ARB);

			glColorMask(GL_FALSE,GL_FALSE,GL_FALSE,GL_FALSE);
			glDepthMask(GL_FALSE);
			glEnable(GL_STENCIL_TEST);
			glStencilFunc(GL_ALWAYS,0,~0);
			glEnable(GL_DEPTH_TEST);
			break;
		case E_RENDER_SHADING:
			glBindProgramARB(GL_VERTEX_PROGRAM_ARB,e_programids[E_PROGRAM_PASSTOFRAGMENT]);
			glEnable(GL_VERTEX_PROGRAM_ARB);
			glEnable(GL_FRAGMENT_PROGRAM_ARB);

			glBlendFunc(GL_ONE,GL_ONE);
			glEnable(GL_BLEND);

			glEnable(GL_STENCIL_TEST);
			glStencilFunc(GL_EQUAL,0,~0);
			glStencilOp(GL_KEEP,GL_KEEP,GL_KEEP);
			glDepthFunc(GL_LEQUAL);

			glEnable(GL_CULL_FACE);
			glEnable(GL_DEPTH_TEST);
			break;
		case E_RENDER_OUTLINES:
			glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
#ifdef E_OUTLINES_SMOOTH
			glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
			glEnable(GL_LINE_SMOOTH);
			glEnable(GL_BLEND);
#endif
			glLineWidth(E_OUTLINES_WIDTH);
			glPolygonOffset(1.0f,1.0f);
			glEnable(GL_POLYGON_OFFSET_LINE);
			glColor3ub(0,0,0);
			glEnable(GL_CULL_FACE);
			glCullFace(GL_FRONT);
			glEnable(GL_DEPTH_TEST);
			break;
	}
}

void e_endpass(e_device_t* device,unsigned int pass)
{
	glPopAttrib();

	switch(pass)
	{
		case E_RENDER_MESHES:
			glDisable(GL_VERTEX_PROGRAM_ARB);
			glDisable(GL_FRAGMENT_PROGRAM_ARB);
			break;
		case E_RENDER_SHADOWS:
			//Disable those extensions manually
			glDisable(GL_VERTEX_PROGRAM_ARB);
			if(glActiveStencilFaceEXT)
			{
				glActiveStencilFaceEXT(GL_FRONT_AND_BACK);
				glDisable(GL_STENCIL_TEST_TWO_SIDE_EXT);
			}

			glColorMask(GL_TRUE,GL_TRUE,GL_TRUE,GL_TRUE);
			glDepthMask(GL_TRUE);
			break;
		case E_RENDER_SHADING:
			glDisable(GL_VERTEX_PROGRAM_ARB);
			glDisable(GL_FRAGMENT_PROGRAM_ARB);
			glDepthFunc(GL_LESS);
			break;
		case E_RENDER_OUTLINES:
			glColor3ub(255,255,255);
			glLineWidth(1.0f);
			break;
	}
}
