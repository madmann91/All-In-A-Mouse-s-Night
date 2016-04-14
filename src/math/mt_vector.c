#include "mt_vector.h"
#include "config.h"

void mt_vec3set(mt_vector3_t v,mt_scalar_t x,mt_scalar_t y,mt_scalar_t z)
{
	v[0]=x;
	v[1]=y;
	v[2]=z;
}

void mt_vec3copy(mt_vector3_t a,const mt_vector3_t b)
{
	a[0]=b[0];
	a[1]=b[1];
	a[2]=b[2];
}

void mt_vec3zero(mt_vector3_t v)
{
	v[0]=(mt_scalar_t)0.0;
	v[1]=(mt_scalar_t)0.0;
	v[2]=(mt_scalar_t)0.0;
}

void mt_vec3norm(mt_vector3_t v)
{
	const mt_scalar_t inv=(mt_scalar_t)1.0/mt_vec3len(v);
	v[0]*=inv;
	v[1]*=inv;
	v[2]*=inv;
}

mt_scalar_t mt_vec3len(const mt_vector3_t a)
{
	return mt_sqrt(a[0]*a[0]+a[1]*a[1]+a[2]*a[2]);
}

mt_scalar_t mt_vec3len2(const mt_vector3_t a)
{
	return a[0]*a[0]+a[1]*a[1]+a[2]*a[2];
}

void mt_vec3min(mt_vector3_t a,const mt_vector3_t b)
{
	if(a[0]>b[0])
		a[0]=b[0];
	if(a[1]>b[1])
		a[1]=b[1];
	if(a[2]>b[2])
		a[2]=b[2];
}

void mt_vec3max(mt_vector3_t a,const mt_vector3_t b)
{
	if(a[0]<b[0])
		a[0]=b[0];
	if(a[1]<b[1])
		a[1]=b[1];
	if(a[2]<b[2])
		a[2]=b[2];
}

void mt_vec3diff(const mt_vector3_t a,const mt_vector3_t b,mt_vector3_t c)
{
	c[0]=a[0]-b[0];
	c[1]=a[1]-b[1];
	c[2]=a[2]-b[2];
}

void mt_vec3sum(const mt_vector3_t a,const mt_vector3_t b,mt_vector3_t c)
{
	c[0]=a[0]+b[0];
	c[1]=a[1]+b[1];
	c[2]=a[2]+b[2];
}

void mt_vec3neg(mt_vector3_t a)
{
	a[0]=-a[0];
	a[1]=-a[1];
	a[2]=-a[2];
}

void mt_vec3scale(mt_vector3_t a,mt_scalar_t s)
{
	a[0]*=s;
	a[1]*=s;
	a[2]*=s;
}

void mt_vec3mulvec3(mt_vector3_t a,const mt_vector3_t b)
{
	a[0]*=b[0];
	a[1]*=b[1];
	a[2]*=b[2];
}

mt_scalar_t mt_vec3dot(const mt_vector3_t a,const mt_vector3_t b)
{
	return a[0]*b[0]+a[1]*b[1]+a[2]*b[2];
}

void mt_vec3cross(const mt_vector3_t a,const mt_vector3_t b,mt_vector3_t c)
{
	c[0]=a[1]*b[2]-a[2]*b[1];
	c[1]=a[2]*b[0]-a[0]*b[2];
	c[2]=a[0]*b[1]-a[1]*b[0];
}

void mt_vec4set(mt_vector4_t v,mt_scalar_t x,mt_scalar_t y,mt_scalar_t z,mt_scalar_t w)
{
	v[0]=x;
	v[1]=y;
	v[2]=z;
	v[3]=w;
}

void mt_vec4copy(mt_vector4_t a,const mt_vector4_t b)
{
	a[0]=b[0];
	a[1]=b[1];
	a[2]=b[2];
	a[3]=b[3];
}

void mt_vec4zero(mt_vector4_t v)
{
	v[0]=(mt_scalar_t)0.0;
	v[1]=(mt_scalar_t)0.0;
	v[2]=(mt_scalar_t)0.0;
	v[3]=(mt_scalar_t)0.0;
}

void mt_vec4norm(mt_vector4_t v)
{
	const mt_scalar_t inv=(mt_scalar_t)1.0/mt_vec4len(v);
	v[0]*=inv;
	v[1]*=inv;
	v[2]*=inv;
	v[3]*=inv;
}

mt_scalar_t mt_vec4len(const mt_vector4_t a)
{
	return mt_sqrt(a[0]*a[0]+a[1]*a[1]+a[2]*a[2]+a[3]*a[3]);
}

mt_scalar_t mt_vec4len2(const mt_vector4_t a)
{
	return a[0]*a[0]+a[1]*a[1]+a[2]*a[2]+a[3]*a[3];
}

void mt_vec4min(mt_vector4_t a,const mt_vector4_t b)
{
	if(a[0]>b[0])
		a[0]=b[0];
	if(a[1]>b[1])
		a[1]=b[1];
	if(a[2]>b[2])
		a[2]=b[2];
	if(a[3]>b[3])
		a[3]=b[3];
}

void mt_vec4max(mt_vector4_t a,const mt_vector4_t b)
{
	if(a[0]<b[0])
		a[0]=b[0];
	if(a[1]<b[1])
		a[1]=b[1];
	if(a[2]<b[2])
		a[2]=b[2];
	if(a[3]<b[3])
		a[3]=b[3];
}

void mt_vec4diff(const mt_vector4_t a,const mt_vector4_t b,mt_vector4_t c)
{
	c[0]=a[0]-b[0];
	c[1]=a[1]-b[1];
	c[2]=a[2]-b[2];
	c[3]=a[3]-b[3];
}

void mt_vec4sum(const mt_vector4_t a,const mt_vector4_t b,mt_vector4_t c)
{
	c[0]=a[0]+b[0];
	c[1]=a[1]+b[1];
	c[2]=a[2]+b[2];
	c[3]=a[3]+b[3];
}

void mt_vec4neg(mt_vector4_t a)
{
	a[0]=-a[0];
	a[1]=-a[1];
	a[2]=-a[2];
	a[3]=-a[3];
}

void mt_vec4scale(mt_vector4_t a,mt_scalar_t s)
{
	a[0]*=s;
	a[1]*=s;
	a[2]*=s;
	a[3]*=s;
}

void mt_vec4mulvec4(mt_vector4_t a,const mt_vector4_t b)
{
	a[0]*=b[0];
	a[1]*=b[1];
	a[2]*=b[2];
	a[3]*=b[3];
}

mt_scalar_t mt_vec4dot(const mt_vector4_t a,const mt_vector4_t b)
{
	return a[0]*b[0]+a[1]*b[1]+a[2]*b[2]+a[3]*b[3];
}

