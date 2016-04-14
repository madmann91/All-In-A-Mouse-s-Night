#include "mt_transform.h"

void mt_transformvector(mt_vector3_t u,const mt_vector3_t v,const mt_transform_t* t)
{
	mt_mat3mulvec3(u,t->rotation,v);
	mt_vec3sum(u,t->position,u);
}

void mt_transformbox(mt_vector3_t newextents,const mt_vector3_t halfextents,const mt_transform_t* t)
{
	mt_matrix3_t absolute;

	mt_mat3abs(absolute,t->rotation);
	mt_mat3mulvec3(newextents,absolute,halfextents);
}

void mt_inversetransform(mt_transform_t* a,const mt_transform_t* b)
{
	mt_mat3transp(a->rotation,b->rotation);
	mt_mat3mulvec3(a->position,a->rotation,b->position);
	mt_vec3neg(a->position);
}

void mt_getmatrix(mt_matrix4_t m,const mt_transform_t* t)
{
	m[0]=t->rotation[0];
	m[1]=t->rotation[3];
	m[2]=t->rotation[6];
	m[3]=0.0f;

	m[4]=t->rotation[1];
	m[5]=t->rotation[4];
	m[6]=t->rotation[7];
	m[7]=0.0f;

	m[8]=t->rotation[2];
	m[9]=t->rotation[5];
	m[10]=t->rotation[8];
	m[11]=0.0f;

	m[12]=t->position[0];
	m[13]=t->position[1];
	m[14]=t->position[2];
	m[15]=1.0f;
}
