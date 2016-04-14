#include "e_particles.h"
#include "e_utils.h"

e_particles_t* e_createparticles(e_device_t* device,unsigned int numelements)
{
	e_particles_t* particles=(e_particles_t*)sys_alloc(sizeof(e_particles_t));
	particles->numactive=0;
	particles->numelements=numelements;
	particles->elements=(e_element_t*)sys_alloc(sizeof(e_element_t)*numelements);
	particles->speed=(mt_scalar_t)1.0;
	return particles;
}

void e_resetparticles(e_particles_t* particles,int flags)
{
	unsigned int i;
	srand(e_getticks());

	particles->numactive=particles->numelements;
	for(i=0;i<particles->numelements;i++)
	{
		e_element_t* element=&particles->elements[i];
		if(flags&E_PARTICLES_WHITE)
		{
			element->color[0]=255;
			element->color[1]=255;
			element->color[2]=255;
			element->color[3]=255;
		}
		if(flags&E_PARTICLES_BLACK)
		{
			element->color[0]=0;
			element->color[1]=0;
			element->color[2]=0;
			element->color[3]=255;
		}
		if(flags&E_PARTICLES_XRAND)
		{
			element->velocity[0]=(mt_scalar_t)((rand()%100)-50)*(mt_scalar_t)0.01;
			element->position[0]=(mt_scalar_t)((rand()%100)-50)*(mt_scalar_t)0.01;
		}
		if(flags&E_PARTICLES_YRAND)
		{
			element->velocity[1]=(mt_scalar_t)((rand()%100)-50)*(mt_scalar_t)0.01;
			element->position[1]=(mt_scalar_t)((rand()%100)-50)*(mt_scalar_t)0.01;
		}
		if(flags&E_PARTICLES_ZRAND)
		{
			element->velocity[2]=(mt_scalar_t)((rand()%100)-50)*(mt_scalar_t)0.01;
			element->position[2]=(mt_scalar_t)((rand()%100)-50)*(mt_scalar_t)0.01;
		}
	}
}

void e_updateparticles(e_particles_t* particles,const mt_vector3_t forces,mt_scalar_t friction,mt_scalar_t dt)
{
	const mt_scalar_t speed=particles->speed*dt;
	unsigned int i;

	for(i=0;i<particles->numelements;i++)
	{
		e_element_t* element=&particles->elements[i];
		mt_scalar_t fade;

		if(!element->color[3])
			continue;

		element->velocity[0]+=(forces[0]-friction*element->velocity[0])*speed;
		element->velocity[1]+=(forces[1]-friction*element->velocity[1])*speed;
		element->velocity[2]+=(forces[2]-friction*element->velocity[2])*speed;

		element->position[0]+=element->velocity[0]*speed;
		element->position[1]+=element->velocity[1]*speed;
		element->position[2]+=element->velocity[2]*speed;

		fade=(mt_scalar_t)255.0*speed;
		if(fade<element->color[3])
			element->color[3]-=(unsigned char)fade;
		else
		{
			element->color[3]=0;
			particles->numactive--;
		}
	}
}

void e_translateparticles(e_particles_t* particles,mt_scalar_t x,mt_scalar_t y,mt_scalar_t z)
{
	unsigned int i;

	for(i=0;i<particles->numelements;i++)
	{
		e_element_t* element=&particles->elements[i];
		element->position[0]+=x;
		element->position[1]+=y;
		element->position[2]+=z;
	}
}

void e_renderparticles(e_device_t* device,const e_particles_t* particles)
{
	glPointSize(2.0f);
	glEnable(GL_POINT_SMOOTH);
	glEnable(GL_BLEND);

	glEnableClientState(GL_VERTEX_ARRAY);
#ifdef E_DOUBLE
	glVertexPointer(3,GL_DOUBLE,sizeof(e_element_t),particles->elements);
#else
	glVertexPointer(3,GL_FLOAT,sizeof(e_element_t),particles->elements);
#endif

	glEnableClientState(GL_COLOR_ARRAY);
	glColorPointer(4,GL_UNSIGNED_BYTE,sizeof(e_element_t),&particles->elements[0].color);

	glDrawArrays(GL_TRIANGLE_STRIP,0,particles->numelements);

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);

	glDisable(GL_POINT_SMOOTH);
	glDisable(GL_BLEND);
	glPointSize(1.0f);
}

void e_destroyparticles(e_particles_t* particles)
{
	sys_free(particles->elements);
	sys_free(particles);
}
