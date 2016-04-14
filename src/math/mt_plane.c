#include "mt_plane.h"

void mt_planeset(mt_plane_t p,const mt_vector3_t n,mt_scalar_t d)
{
	mt_vec3copy((mt_scalar_t*)p,n);
	p[3]=d;
}

void mt_planecopy(mt_plane_t a,const mt_plane_t b)
{
	a[0]=b[0];
	a[1]=b[1];
	a[2]=b[2];
	a[3]=b[3];
}

mt_scalar_t mt_planedistance(const mt_plane_t p,const mt_vector3_t v)
{
	return mt_vec3dot((mt_scalar_t*)p,v)-p[3];
}

void mt_triangleplane(const mt_vector3_t a,const mt_vector3_t b,const mt_vector3_t c,mt_plane_t p)
{	
	mt_vector3_t edges[2];

	mt_vec3diff(b,a,edges[0]);
	mt_vec3diff(c,a,edges[1]);
	mt_vec3cross(edges[0],edges[1],(mt_scalar_t*)p);

	p[3]=mt_vec3dot((mt_scalar_t*)p,a);
}

void mt_trianglenormal(const mt_vector3_t a,const mt_vector3_t b,const mt_vector3_t c,mt_vector3_t n)
{
	mt_vector3_t edges[2];
	mt_vec3diff(b,a,edges[0]);
	mt_vec3diff(c,a,edges[1]);
	mt_vec3cross(edges[0],edges[1],n);
}
