#include "sys_common.h"
#include "g_character.h"
#include "g_controller.h"
#include "g_variables.h"

static void g_usercontrol(g_character_t* character,mt_scalar_t dt)
{
	g_controller_t* controller=character->controller;
	g_usercontroller_t* user=&controller->user;
	const unsigned char* buffer=e_getkeyboard(user->device);
	const int middle[2]={user->device->width/2,user->device->height/2};
	mt_scalar_t cosinus,sinus;
	int x,y;

	//Change player's direction with mouse input
	e_getmouse(user->device,&x,&y);
	character->heading+=(x-middle[0])*user->sensitivity;
	character->pitch+=(middle[1]-y)*user->sensitivity;
	if(character->pitch>MT_PI/6)
		character->pitch=MT_PI/6;
	if(character->pitch<-MT_PI/4)
		character->pitch=-MT_PI/4;

	cosinus=mt_cos(character->heading)*g_step;
	sinus=mt_sin(character->heading)*g_step;

	if(buffer[user->controls[G_CONTROL_TURNLEFT]])
		character->heading-=user->sensitivity;
	if(buffer[user->controls[G_CONTROL_TURNRIGHT]])
		character->heading+=user->sensitivity;

	//Move character if possible
	if(character->flags&G_CHARACTER_CANJUMP)
	{
		character->state=G_CHARACTER_WAITING;
		if(buffer[user->controls[G_CONTROL_STRAFELEFT]])
		{
			character->velocity[0]+=sinus;
			character->velocity[2]-=cosinus;
			character->state=G_CHARACTER_RUNNING;
		}
		if(buffer[user->controls[G_CONTROL_STRAFERIGHT]])
		{
			character->velocity[0]-=sinus;
			character->velocity[2]+=cosinus;
			character->state=G_CHARACTER_RUNNING;
		}
		if(buffer[user->controls[G_CONTROL_FORWARD]])
		{
			character->velocity[0]+=cosinus;
			character->velocity[2]+=sinus;
			character->state=G_CHARACTER_RUNNING;
		}
		if(buffer[user->controls[G_CONTROL_BACKWARD]])
		{
			character->velocity[0]-=cosinus;
			character->velocity[2]-=sinus;
			character->state=G_CHARACTER_RUNNING;
		}
		if(buffer[user->controls[G_CONTROL_JUMP]])
		{
			character->velocity[1]=g_jump;
			character->state=G_CHARACTER_JUMPING;
		}

		if(character->state==G_CHARACTER_RUNNING)
			character->velocity[1]+=g_hstep;
	}
	else
		character->state=G_CHARACTER_JUMPING;
}

static void g_aicontrol(g_character_t* character,mt_scalar_t dt)
{
	/*g_controller_t* controller=character->controller;
	g_aicontroller_t* ai=&controller->info.ai;

	if(character->flags&G_CHARACTER_CANJUMP)
	{
		mt_vector3_t velocity;

		//Compute the new velocity
		mt_vec3diff(ai->location,character->transform.position,velocity);

		//Compute the character's heading
		if(velocity[0]>0)
			character->heading=mt_atan(velocity[2]/velocity[0]);
		else
			character->heading=mt_atan(velocity[2]/velocity[0])+MT_PI;

		//Scale the velocity to fit the maximum step speed
		mt_vec3scale(velocity,g_step/mt_vec3len(velocity));

		//Update the character's velocity
		character->velocity[0]+=velocity[0];
		character->velocity[1]+=(velocity[1]>0)?velocity[1]:g_hstep;
		character->velocity[2]+=velocity[2];

		character->state=G_CHARACTER_RUNNING;
	}
	else
		character->state=G_CHARACTER_JUMPING;*/

	//Just a random controller
	/*mt_scalar_t cosinus=mt_cos(character->heading)*g_step;
	mt_scalar_t sinus=mt_sin(character->heading)*g_step;
	
	if(character->flags&G_CHARACTER_CANJUMP)
	{
		int i=rand()%4;
		switch(i)
		{
			case 0:
			case 1:
				character->velocity[0]+=cosinus*0.001f;
				character->velocity[2]+=sinus*0.001f;
				character->state=G_CHARACTER_RUNNING;
				break;
			case 2:
				character->heading+=(mt_scalar_t)0.1;
				break;
			case 3:
				character->heading-=(mt_scalar_t)0.1;
				break;
		}
	}
	else
		character->state=G_CHARACTER_JUMPING;*/

}

g_controller_t* g_createcontroller(unsigned int type)
{
	g_controller_t* controller=0;
	switch(type)
	{
		case G_CONTROLLER_USER:
			controller=(g_controller_t*)sys_alloc(sizeof(g_controller_t));
			memset(controller,0,sizeof(g_controller_t));
			controller->type=type;
			controller->func=g_usercontrol;
			break;
		case G_CONTROLLER_AI:
			controller=(g_controller_t*)sys_alloc(sizeof(g_controller_t));
			memset(controller,0,sizeof(g_controller_t));
			controller->type=type;
			controller->func=g_aicontrol;
			break;
	}
	return controller;
}

void g_destroycontroller(g_controller_t* controller)
{
	sys_free(controller);
}
