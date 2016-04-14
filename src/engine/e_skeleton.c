#include "e_skeleton.h"

const e_filetype_t e_skeletonfile={('S'+('K'<<8)+('T'<<16)+('1'<<24)),1};

//These functions are helpful when there's a need to blend animations
static void e_interpolate(e_frame_t* c,const e_frame_t* a,const e_frame_t* b,mt_scalar_t t)
{
	const mt_scalar_t f=(mt_scalar_t)1.0-t;

	c->pos[0]=f*a->pos[0]+t*b->pos[0];
	c->pos[1]=f*a->pos[1]+t*b->pos[1];
	c->pos[2]=f*a->pos[2]+t*b->pos[2];
	mt_quatslerp(c->quat,a->quat,b->quat,t);
	c->len=f*a->len+t*b->len;
}

static void e_blend(e_frame_t* c,const e_frame_t* a,const e_frame_t* b,mt_scalar_t t)
{
	const mt_scalar_t f=(mt_scalar_t)1.0-t;

	c->pos[0]=f*a->pos[0]+t*b->pos[0];
	c->pos[1]=f*a->pos[1]+t*b->pos[1];
	c->pos[2]=f*a->pos[2]+t*b->pos[2];
	mt_quatnlerp(c->quat,a->quat,b->quat,t);
	c->len=f*a->len+t*b->len;
}

e_skeletonbuffer_t* e_createskeletonbuffer(e_skeleton_t* skeleton)
{
	e_skeletonbuffer_t* buffer=(e_skeletonbuffer_t*)sys_alloc(sizeof(e_skeletonbuffer_t));
	buffer->current=(mt_matrix4_t*)sys_alloc(sizeof(mt_matrix4_t)*skeleton->numbones);
	buffer->state=(e_frame_t*)sys_alloc(sizeof(e_frame_t)*skeleton->numbones);
	return buffer;
}

void e_updateskeletonbuffer(e_skeletonbuffer_t* buffer,const e_skeleton_t* skeleton,const e_animation_t* anim)
{
	unsigned int i;

	for(i=0;i<skeleton->numbones;i++)
	{
		const int bone=skeleton->bones[i];
		e_frame_t state;

		//We first animate the skeleton in bone space
		if(!anim->blend)
		{
			const e_frame_t* frame1=&skeleton->frames[anim->anim1[0]][i];
			const e_frame_t* frame2=&skeleton->frames[anim->anim1[1]][i];

			e_interpolate(&state,frame1,frame2,anim->factor1);
		}
		else
		{
			e_frame_t temp1,temp2;

			e_interpolate(&temp1,&skeleton->frames[anim->anim1[0]][i],&skeleton->frames[anim->anim1[1]][i],anim->factor1);
			e_interpolate(&temp2,&skeleton->frames[anim->anim2[0]][i],&skeleton->frames[anim->anim2[1]][i],anim->factor2);
			
			e_blend(&state,&temp1,&temp2,anim->lambda);
		}

		//Then we transform the bone space in object space
		if(bone>=0)
		{
			e_frame_t* frame=&buffer->state[i];
			const e_frame_t* parent=&buffer->state[bone];

			mt_quatmulquat(frame->quat,parent->quat,state.quat);
			mt_quatmulvec3(frame->pos,parent->quat,state.pos);

			frame->pos[0]+=parent->pos[0];
			frame->pos[1]+=parent->pos[1];
			frame->pos[2]+=parent->pos[2];

			frame->len=state.len;
		}
		else
			buffer->state[i]=state;

		//Compute the state relative to the base frame
		mt_quatmulquat(state.quat,buffer->state[i].quat,skeleton->base[i].quat);
		mt_quatmulvec3(state.pos,buffer->state[i].quat,skeleton->base[i].pos);
		state.pos[0]+=buffer->state[i].pos[0];
		state.pos[1]+=buffer->state[i].pos[1];
		state.pos[2]+=buffer->state[i].pos[2];

		//Save it as a 4*4 matrix
		mt_quattomat4(buffer->current[i],state.quat,state.pos);
	}
}

void e_destroyskeletonbuffer(e_skeletonbuffer_t* buffer)
{
	sys_free(buffer->state);
	sys_free(buffer->current);
	sys_free(buffer);
}

e_skeleton_t* e_loadskeleton(e_device_t* device,fs_handle_t file)
{
	e_skeleton_t* skeleton;
	e_filetype_t header;
	unsigned int i;

	fs_read(&header,sizeof(e_filetype_t),file);
	if(!e_checkfiletype(&header,&e_skeletonfile))
		return 0;

	skeleton=(e_skeleton_t*)sys_alloc(sizeof(e_skeleton_t));

	fs_read(&skeleton->numbones,sizeof(unsigned int),file);
	fs_read(&skeleton->numframes,sizeof(unsigned int),file);
	fs_read(&skeleton->numactions,sizeof(unsigned int),file);
	fs_read(skeleton->hands,sizeof(unsigned int)*2,file);

	skeleton->bones=(int*)sys_alloc(sizeof(int)*skeleton->numbones);
	fs_read(skeleton->bones,sizeof(int)*skeleton->numbones,file);

	skeleton->frames=(e_frame_t**)sys_alloc(sizeof(e_frame_t*)*skeleton->numframes);
	for(i=0;i<skeleton->numframes;i++)
	{
		skeleton->frames[i]=(e_frame_t*)sys_alloc(sizeof(e_frame_t)*skeleton->numbones);
		fs_read(skeleton->frames[i],sizeof(e_frame_t)*skeleton->numbones,file);
	}

	skeleton->actions=(e_action_t*)sys_alloc(sizeof(e_action_t)*skeleton->numactions);
	fs_read(skeleton->actions,sizeof(e_action_t)*skeleton->numactions,file);

	//We need to compute the inverse of the base frame
	skeleton->base=(e_frame_t*)sys_alloc(sizeof(e_frame_t)*skeleton->numbones);
	for(i=0;i<skeleton->numbones;i++)
	{
		const e_frame_t* frame=&skeleton->frames[0][i];
		e_frame_t* base=&skeleton->base[i];

		mt_quatinv(base->quat,frame->quat);
		mt_quatmulvec3(base->pos,base->quat,frame->pos);
		mt_vec3neg(base->pos);
	}

	return skeleton;
}

void e_destroyskeleton(e_skeleton_t* skeleton)
{
	unsigned int i;

	sys_free(skeleton->bones);
	for(i=0;i<skeleton->numframes;i++)
		sys_free(skeleton->frames[i]);
	sys_free(skeleton->frames);
	sys_free(skeleton->base);
	sys_free(skeleton->actions);
	sys_free(skeleton);
}

void e_renderskeleton(e_device_t* device,const e_skeleton_t* skeleton,const e_skeletonbuffer_t* buffer,const mt_matrix4_t matrix)
{
	unsigned int i;

	glPushMatrix();
#ifndef E_DOUBLE
	glMultMatrixf(matrix);
#else
	glMultMatrixd(matrix);
#endif

	glLineWidth(1.5f);
	glBegin(GL_LINES);
	for(i=0;i<skeleton->numbones;i++)
	{
		mt_vector3_t v;
		mt_quatori(v,buffer->state[i].quat);

		v[0]=(v[0]*buffer->state[i].len)+buffer->state[i].pos[0];
		v[1]=(v[1]*buffer->state[i].len)+buffer->state[i].pos[1];
		v[2]=(v[2]*buffer->state[i].len)+buffer->state[i].pos[2];

		glColor3f(1.0f,0.0f,0.0f);
		glVertex3fv(buffer->state[i].pos);

		glColor3f(0.0f,1.0f,0.0f);
		glVertex3fv(v);
	}
	glColor3f(1.0f,1.0f,1.0f);
	glEnd();
	glLineWidth(1.0f);

	glPopMatrix();
}

void e_getbonematrix(mt_matrix4_t matrix,unsigned int bone,const e_skeleton_t* skeleton,const e_skeletonbuffer_t* buffer)
{
	const e_frame_t* frame=&buffer->state[bone];
	const e_frame_t* base=&skeleton->base[bone];
	mt_quaternion_t q;
	mt_vector3_t dir;

	mt_quatmulquat(q,frame->quat,base->quat);

	//Get the position of the character's hand (and its orientation)
	mt_quatori(dir,frame->quat);
	mt_quattomat4(matrix,q,frame->pos);

	//Add the the bone position
	matrix[12]+=dir[0]*frame->len;
	matrix[13]+=dir[1]*frame->len;
	matrix[14]+=dir[2]*frame->len;
}

void e_nextanimation(e_animation_t* anim,const e_skeleton_t* skeleton,unsigned int action)
{
	if(anim->blend)
	{
		//We don't want to play an action currently playing
		if(anim->action2!=action)
		{
			anim->anim1[0]=anim->anim2[0];
			anim->anim1[1]=anim->anim2[1];
			anim->factor1=anim->factor2;
			anim->action1=anim->action2;

			anim->anim2[0]=skeleton->actions[action].begin;
			anim->anim2[1]=anim->anim2[0]+1;	//There's at least one frame after the beginning of an action
			anim->factor2=(mt_scalar_t)0.0;
			anim->action2=action;

			anim->lambda=(mt_scalar_t)0.0;
		}
	}
	else
	{
		if(anim->action1!=action)
		{
			anim->anim2[0]=skeleton->actions[action].begin;
			anim->anim2[1]=anim->anim2[0]+1;
			anim->factor2=(mt_scalar_t)0.0;
			anim->action2=action;

			anim->blend=1;
			anim->lambda=(mt_scalar_t)0.0;
		}
	}
}

//This macro makes the frame counter advance
#define E_ADVANCE(factor,anim,action) \
	if(factor>=(mt_scalar_t)1.0) \
	{ \
		const int num=action->end-action->begin+1; \
		int times,intpart; \
		intpart=(int)factor; \
		times=(anim[0]-action->begin+intpart)%num; \
		factor-=(mt_scalar_t)intpart; \
		anim[0]=action->begin+times; \
		anim[1]=anim[0]+1; \
		if(anim[1]>action->end) \
			anim[1]=action->begin; \
	}

void e_playanimation(e_animation_t* anim,const e_skeleton_t* skeleton,mt_scalar_t step,mt_scalar_t lambda)
{
	if(anim->blend)
	{
		const e_action_t* action1=&skeleton->actions[anim->action1];
		const e_action_t* action2=&skeleton->actions[anim->action2];

		anim->lambda+=lambda*(action1->speed+action2->speed);
		if(anim->lambda>=(mt_scalar_t)1.0)
		{
			//The animation has been completly blended
			//We have now only one animation playing
			anim->blend=0;
			anim->anim1[0]=anim->anim2[0];
			anim->anim1[1]=anim->anim2[1];
			anim->factor1=anim->factor2+step*action2->speed;
			anim->action1=anim->action2;
			
			//Go to next frame if needed
			E_ADVANCE(anim->factor1,anim->anim1,action2);
		}
		else
		{
			const e_action_t* action1=&skeleton->actions[anim->action1];
			const e_action_t* action2=&skeleton->actions[anim->action2];

			//Increment the two factors by step
			anim->factor1+=step*action1->speed;
			anim->factor2+=step*action2->speed;

			//For each action, go to the next frame if needed
			E_ADVANCE(anim->factor1,anim->anim1,action1);
			E_ADVANCE(anim->factor2,anim->anim2,action2);
		}
	}
	else
	{
		const e_action_t* action1=&skeleton->actions[anim->action1];

		anim->factor1+=step*action1->speed;

		E_ADVANCE(anim->factor1,anim->anim1,action1);
	}
}

#define E_STOP(factor,anim,action) \
	if(factor>=(mt_scalar_t)1.0) \
	{ \
		if(anim[1]<action->end) \
		{ \
			unsigned int times=(unsigned int)factor; \
			factor-=(mt_scalar_t)times; \
			times+=anim[0]; \
			if(times<action1->end) \
			{ \
				anim[0]=times; \
				anim[1]=anim[0]+1; \
			} \
			else \
			{ \
				anim[0]=action->end-1; \
				anim[1]=action->end; \
			} \
		} \
		else \
			factor=(mt_scalar_t)1.0; \
	}

void e_stopanimation(e_animation_t* anim,const e_skeleton_t* skeleton,mt_scalar_t step,mt_scalar_t lambda)
{
	if(anim->blend)
	{
		const e_action_t* action1=&skeleton->actions[anim->action1];
		const e_action_t* action2=&skeleton->actions[anim->action2];

		anim->lambda+=lambda*(action1->speed+action2->speed);
		if(anim->lambda>=(mt_scalar_t)1.0)
		{
			//The animation has been completly blended
			//We have now only one animation playing
			anim->blend=0;
			anim->anim1[0]=anim->anim2[0];
			anim->anim1[1]=anim->anim2[1];
			anim->factor1=anim->factor2+step*action2->speed;
			anim->action1=anim->action2;
			
			//Go to next frame if the end has not been reached
			E_STOP(anim->factor1,anim->anim1,action2);
		}
		else
		{
			//Both animations have to be played
			anim->factor1+=step*action1->speed;
			anim->factor2+=step*action2->speed;

			//The first animation is played continuously
			E_ADVANCE(anim->factor1,anim->anim1,action1);
			//But the second will stop when the end frame has been reached
			E_STOP(anim->factor2,anim->anim2,action2);
		}
	}
	else
	{
		const e_action_t* action1=&skeleton->actions[anim->action1];

		anim->factor1+=step*action1->speed;

		//Here we stop when the end frame is reached
		E_STOP(anim->factor1,anim->anim1,action1);
	}
}
