#include "config.h"
#include "mt_collision.h"
#include "mt_plane.h"

int mt_frustumtobox(const mt_plane_t frustum[6],const mt_vector3_t min,const mt_vector3_t max)
{
	unsigned int i;
	mt_vector3_t v;

	//Check the 6 planes
	for(i=0;i<6;i++)
	{
		v[0]=(frustum[i][0]<0)?min[0]:max[0];
		v[1]=(frustum[i][1]<0)?min[1]:max[1];
		v[2]=(frustum[i][2]<0)?min[2]:max[2];

		//We only consider the extreme vertices of the box
		if(frustum[i][0]*v[0]+frustum[i][1]*v[1]+frustum[i][2]*v[2]-frustum[i][3]<0)
			return 0;
	}

	return 1;
}

int mt_raytotriangle(const mt_vector3_t start,const mt_vector3_t dir,const mt_vector3_t a,const mt_vector3_t b,const mt_vector3_t c,mt_scalar_t* t0)
{
	//Adapted from A Fast Ray Triangle Intersection Test by Tomas Akenine Moller
	mt_vector3_t e1,e2,tvec,pvec,qvec;
	mt_scalar_t det,inv,u,v;

	mt_vec3diff(b,a,e1);
	mt_vec3diff(c,a,e2);

	mt_vec3cross(dir,e2,pvec);

	det=mt_vec3dot(e1,pvec);

	if(mt_fabs(det)<MT_EPSILON)
		return 0;

	inv=(mt_scalar_t)1.0/det;

	mt_vec3diff(start,a,tvec);

	u=mt_vec3dot(tvec,pvec)*inv;
	if(u<(mt_scalar_t)0.0||u>(mt_scalar_t)1.0)
		return 0;

	mt_vec3cross(tvec,e1,qvec);

	v=mt_vec3dot(dir,qvec)*inv;
	if(v<(mt_scalar_t)0.0||u+v>(mt_scalar_t)1.0)
		return 0;

	*t0=mt_vec3dot(e2,qvec)*inv;

	return 1;
}

int mt_raytobox(const mt_vector3_t start,const mt_vector3_t invdir,const mt_vector3_t min,const mt_vector3_t max,mt_scalar_t* t0,mt_scalar_t* t1)
{
	//Ray-box algorithm inspired by the Smits algorithm
	const mt_scalar_t tymin=(invdir[1]>=0)?(min[1]-start[1])*invdir[1]:(max[1]-start[1])*invdir[1];
	const mt_scalar_t tymax=(invdir[1]>=0)?(max[1]-start[1])*invdir[1]:(min[1]-start[1])*invdir[1];

	mt_scalar_t tmin=(invdir[0]>=0)?(min[0]-start[0])*invdir[0]:(max[0]-start[0])*invdir[0];
	mt_scalar_t tmax=(invdir[0]>=0)?(max[0]-start[0])*invdir[0]:(min[0]-start[0])*invdir[0];

	mt_scalar_t tzmin,tzmax;

	if(tmin>tymax||tymin>tmax)
		return 0;

	if(tymin<tmin)
		tmin=tymin;
	if(tymax>tmax)
		tmax=tymax;

	tzmin=(invdir[2]>=0)?(min[2]-start[2])*invdir[2]:(max[2]-start[2])*invdir[2];
	tzmax=(invdir[2]>=0)?(max[2]-start[2])*invdir[2]:(min[2]-start[2])*invdir[2];

	if(tmin>tzmax||tzmin>tmax)
		return 0;

	if(tzmin<tmin)
		tmin=tzmin;
	if(tzmax>tmax)
		tmax=tzmax;

	*t0=tmin;
	*t1=tmax;

	return 1;
}

#define MT_FINDMINMAX(x0,x1,x2,min,max) \
	min=max=x0;   \
	if(x1<min) min=x1;\
	if(x1>max) max=x1;\
	if(x2<min) min=x2;\
	if(x2>max) max=x2;

#define MT_AXISTEST_X(edge,fa,fb,v) \
	p0=edge[2]*v0[1]-edge[1]*v0[2];	\
	p1=edge[2]*v[1]-edge[1]*v[2];	\
	min=(p0<p1)?p0:p1; \
	max=(p0<p1)?p1:p0; \
	rad=fa*halfextents[1]+fb*halfextents[2]; \
	if(min>rad||max<-rad) return 0;

#define MT_AXISTEST_Y(edge,fa,fb,v) \
	p0=-edge[2]*v0[0]+edge[0]*v0[2]; \
	p1=-edge[2]*v[0]+edge[0]*v[2]; \
	min=(p0<p1)?p0:p1; \
	max=(p0<p1)?p1:p0; \
	rad=fa*halfextents[0]+fb*halfextents[2]; \
	if(min>rad||max<-rad) return 0;

#define MT_AXISTEST_Z(edge,fa,fb,v) \
	p0=edge[1]*v[0]-edge[0]*v[1];	\
	p1=edge[1]*v1[0]-edge[0]*v1[1];	\
	min=(p0<p1)?p0:p1; \
	max=(p0<p1)?p1:p0; \
	rad=fa*halfextents[0]+fb*halfextents[1]; \
	if(min>rad||max<-rad) return 0;

int mt_triangletobox(const mt_vector3_t v0,const mt_vector3_t v1,const mt_vector3_t v2,const mt_vector3_t halfextents)
{
	//Adapted from A Fast AABB-triangle Overlap Test by Tomas Akenine Moller (I changed the plane-box test, and the macros)
	mt_vector3_t normal,e0,e1,e2;
	mt_scalar_t min,max,p0,p1,rad,fex,fey,fez;

	mt_vec3diff(v1,v0,e0);
	mt_vec3diff(v2,v1,e1);
	mt_vec3diff(v0,v2,e2);

	fex=mt_fabs(e0[0]);
	fey=mt_fabs(e0[1]);
	fez=mt_fabs(e0[2]);
	MT_AXISTEST_X(e0,fez,fey,v2);
	MT_AXISTEST_Y(e0,fez,fex,v2);
	MT_AXISTEST_Z(e0,fey,fex,v2);

	fex=mt_fabs(e1[0]);
	fey=mt_fabs(e1[1]);
	fez=mt_fabs(e1[2]);
	MT_AXISTEST_X(e1,fez,fey,v2);
	MT_AXISTEST_Y(e1,fez,fex,v2);
	MT_AXISTEST_Z(e1,fey,fex,v0);

	fex=mt_fabs(e2[0]);
	fey=mt_fabs(e2[1]);
	fez=mt_fabs(e2[2]);
	MT_AXISTEST_X(e2,fez,fey,v1);
	MT_AXISTEST_Y(e2,fez,fex,v1);
	MT_AXISTEST_Z(e2,fey,fex,v2);

	MT_FINDMINMAX(v0[0],v1[0],v2[0],min,max);
	if(min>halfextents[0]||max<-halfextents[0]) return 0;

	MT_FINDMINMAX(v0[1],v1[1],v2[1],min,max);
	if(min>halfextents[1]||max<-halfextents[1]) return 0;

	MT_FINDMINMAX(v0[2],v1[2],v2[2],min,max);
	if(min>halfextents[2]||max<-halfextents[2]) return 0;

	mt_vec3cross(e0,e1,normal);
	p0=mt_vec3dot(v0,normal);
	rad=halfextents[0]*mt_fabs(normal[0])+halfextents[1]*mt_fabs(normal[1])+halfextents[2]*mt_fabs(normal[2]);

	return (mt_fabs(p0)<rad);
}

#define MT_SWEEPTEST(rad,w,nx,ny,nz) \
	if(w>0) \
	{ \
		t=-(rad+max)/w; \
		max+=w; \
	} \
	else \
	{ \
		t=(rad-min)/w; \
		min+=w; \
	} \
	if(min>rad||max<-rad) return 0; \
	if(t>tmax&&t<=(mt_scalar_t)1.0)\
	{\
		tmax=t; \
		n[0]=nx; \
		n[1]=ny; \
		n[2]=nz; \
	}

#define MT_SWEEPAXISTEST_X(edge,fa,fb,v) \
	w=edge[2]*velocity[1]-edge[1]*velocity[2]; \
	p0=edge[2]*v0[1]-edge[1]*v0[2];	\
	p1=edge[2]*v[1]-edge[1]*v[2]; \
	min=(p0<p1)?p0:p1; \
	max=(p0<p1)?p1:p0; \
	rad=fa*halfextents[1]+fb*halfextents[2]; \
	MT_SWEEPTEST(rad,w,0,edge[2],-edge[1]);

#define MT_SWEEPAXISTEST_Y(edge,fa,fb,v) \
	w=-edge[2]*velocity[0]+edge[0]*velocity[2]; \
	p0=-edge[2]*v0[0]+edge[0]*v0[2]; \
	p1=-edge[2]*v[0]+edge[0]*v[2]; \
	min=(p0<p1)?p0:p1; \
	max=(p0<p1)?p1:p0; \
	rad=fa*halfextents[0]+fb*halfextents[2]; \
	MT_SWEEPTEST(rad,w,-edge[2],0,-edge[0]);

#define MT_SWEEPAXISTEST_Z(edge,fa,fb,v) \
	w=edge[1]*velocity[0]-edge[0]*velocity[1]; \
	p0=edge[1]*v1[0]-edge[0]*v1[1];	\
	p1=edge[1]*v[0]-edge[0]*v[1]; \
	min=(p1<p0)?p1:p0; \
	max=(p1<p0)?p0:p1; \
	rad=fa*halfextents[0]+fb*halfextents[1]; \
	MT_SWEEPTEST(rad,w,edge[1],-edge[0],0);

#define MT_EDGETEST_X \
	rad=fez*halfextents[1]+fey*halfextents[2]; \
	p0=-velocity[2]*v0[1]+velocity[1]*v0[2]; \
	p1=-velocity[2]*v1[1]+velocity[1]*v1[2]; \
	p2=-velocity[2]*v2[1]+velocity[1]*v2[2]; \
	MT_FINDMINMAX(p0,p1,p2,min,max); \
	if(min>rad||max<-rad) return 0;

#define MT_EDGETEST_Y \
	rad=fez*halfextents[0]+fex*halfextents[2]; \
	p0=velocity[2]*v0[0]-velocity[0]*v0[2]; \
	p1=velocity[2]*v1[0]-velocity[0]*v1[2]; \
	p2=velocity[2]*v2[0]-velocity[0]*v2[2]; \
	MT_FINDMINMAX(p0,p1,p2,min,max); \
	if(min>rad||max<-rad) return 0;

#define MT_EDGETEST_Z \
	rad=fey*halfextents[0]+fex*halfextents[1]; \
	p0=-velocity[1]*v0[0]+velocity[0]*v0[1]; \
	p1=-velocity[1]*v1[0]+velocity[0]*v1[1]; \
	p2=-velocity[1]*v2[0]+velocity[0]*v2[1]; \
	MT_FINDMINMAX(p0,p1,p2,min,max); \
	if(min>rad||max<-rad) return 0;

int mt_sweeptriangletobox(const mt_vector3_t v0,const mt_vector3_t v1,const mt_vector3_t v2,const mt_vector3_t velocity,const mt_vector3_t halfextents,mt_scalar_t* t0,mt_vector3_t n)
{
	//This test is based on the same idea as the one before : the SAT theorem.
	//However, this test is slower since we compute the time of intersection, and since we have to check the swept
	//triangle axes (along the velocity) against the box (3 tests more).
	//I'd like to thank Geometric Tools for their great tutorial, and of course, Thomas Moller for his original idea.
	mt_vector3_t normal,e0,e1,e2;
	mt_scalar_t rad,w,p0,p1,p2,min,max,fex,fey,fez;
	mt_scalar_t tmax=0,t;

	mt_vec3diff(v1,v0,e0);
	mt_vec3diff(v2,v1,e1);
	mt_vec3diff(v0,v2,e2);

	//Cross axes
	fex=mt_fabs(e0[0]);
	fey=mt_fabs(e0[1]);
	fez=mt_fabs(e0[2]);
	MT_SWEEPAXISTEST_X(e0,fez,fey,v2);
	MT_SWEEPAXISTEST_Y(e0,fez,fex,v2);
	MT_SWEEPAXISTEST_Z(e0,fey,fex,v2);

	fex=mt_fabs(e1[0]);
	fey=mt_fabs(e1[1]);
	fez=mt_fabs(e1[2]);
	MT_SWEEPAXISTEST_X(e1,fez,fey,v2);
	MT_SWEEPAXISTEST_Y(e1,fez,fex,v2);
	MT_SWEEPAXISTEST_Z(e1,fey,fex,v0);

	fex=mt_fabs(e2[0]);
	fey=mt_fabs(e2[1]);
	fez=mt_fabs(e2[2]);
	MT_SWEEPAXISTEST_X(e2,fez,fey,v1);
	MT_SWEEPAXISTEST_Y(e2,fez,fex,v1);
	MT_SWEEPAXISTEST_Z(e2,fey,fex,v2);

	//Box axes (3 tests)
	MT_FINDMINMAX(v0[0],v1[0],v2[0],min,max);
	MT_SWEEPTEST(halfextents[0],velocity[0],1,0,0);

	MT_FINDMINMAX(v0[1],v1[1],v2[1],min,max);
	MT_SWEEPTEST(halfextents[1],velocity[1],0,1,0);

	MT_FINDMINMAX(v0[2],v1[2],v2[2],min,max);
	MT_SWEEPTEST(halfextents[2],velocity[2],0,0,1);

	//Swept triangle axes (3 tests more)
	fex=mt_fabs(velocity[0]);
	fey=mt_fabs(velocity[1]);
	fez=mt_fabs(velocity[2]);
	MT_EDGETEST_X;
	MT_EDGETEST_Y;
	MT_EDGETEST_Z;

	//Plane test
	mt_vec3cross(e0,e1,normal);
	w=mt_vec3dot(normal,velocity);
	p0=mt_vec3dot(v0,normal);
	rad=halfextents[0]*mt_fabs(normal[0])+halfextents[1]*mt_fabs(normal[1])+halfextents[2]*mt_fabs(normal[2]);
	min=max=p0;
	MT_SWEEPTEST(rad,w,normal[0],normal[1],normal[2]);

	//Setup the collision normal if it hasn't been set before
	if(tmax==0)
		mt_vec3copy(n,normal);
	*t0=tmax;

	return 1;
}

int mt_sweepboxtobox(const mt_vector3_t min1,const mt_vector3_t max1,const mt_vector3_t invvelocity,const mt_vector3_t min2,const mt_vector3_t max2,mt_scalar_t* t0,mt_scalar_t* t1,mt_vector3_t n)
{
	mt_scalar_t tmin,tmax,k0,k1;
	unsigned int i;
	char axis;

	tmax=MT_MAXIMUM;
	tmin=-MT_MAXIMUM;
	axis=0;

	//Check the 3 axes
	for(i=0;i<3;i++)
	{
		if(invvelocity[i]>0)
		{
			k0=(min2[i]-max1[i])*invvelocity[i];
			k1=(max2[i]-min1[i])*invvelocity[i];
		}
		else
		{
			k0=(max2[i]-min1[i])*invvelocity[i];
			k1=(min2[i]-max1[i])*invvelocity[i];
		}

		if(tmax>k1)
			tmax=k1;
		if(tmin<k0)
		{
			tmin=k0;
			axis=i;
		}
	}

	//Return if no collision was found
	if(tmax<tmin)
		return 0;

	*t0=tmin;
	*t1=tmax;
	mt_vec3zero(n);
	n[axis]=1;

	return 1;
}

#define MT_SPHERETOEDGE(e,v) \
	mt_vec3diff(pos,v,pt); \
	mt_vec3cross(e,pt,a); \
	mt_vec3cross(e,vel,b); \
	len=mt_vec3len2(e); \
	c[0]=mt_vec3len2(b); \
	c[1]=2*mt_vec3dot(a,b); \
	c[2]=mt_vec3len2(a)-len; \
	if(mt_trinom(c[0],c[1],c[2],t1,&t0)) \
	{ \
		const mt_scalar_t f=(mt_vec3dot(e,vel)*t0+mt_vec3dot(pt,e))/len; \
		if(f>=0&&f<=1) \
		{ \
			pt[0]=v[0]+e[0]*f; \
			pt[1]=v[1]+e[1]*f; \
			pt[2]=v[2]+e[2]*f; \
			mt_vec3diff(pt,pos,n); \
			found=1; \
			t1=t0; \
		} \
	}

#define MT_SPHERETOPOINT(v) \
	mt_vec3diff(pos,v,pt); \
	c[1]=2*mt_vec3dot(pt,vel); \
	c[2]=mt_vec3len2(pt)-1; \
	if(mt_trinom(c[0],c[1],c[2],t1,&t0)) \
	{ \
		mt_vec3diff(v,pos,n); \
		found=1; \
		t1=t0; \
	}

int mt_sweepspheretotriangle(const mt_vector3_t pos,const mt_vector3_t vel,const mt_vector3_t v0,const mt_vector3_t v1,const mt_vector3_t v2,mt_scalar_t* t,mt_vector3_t n)
{
	mt_scalar_t pvel,dist,len;
	mt_vector3_t e0,e1,e2;
	mt_vector3_t a,b,c;
	mt_vector3_t pt,s,d;
	mt_scalar_t t0,t1;
	mt_plane_t p;	
	char found=0;

	//Compute edges
	mt_vec3diff(v0,v1,e0);
	mt_vec3diff(v1,v2,e1);
	mt_vec3diff(v2,v0,e2);

	//Compute normal
	mt_vec3cross(e0,e1,p);
	mt_vec3norm(p);
	p[3]=mt_vec3dot(v0,p);

	//Project velocity on normal
	pvel=mt_vec3dot(vel,p);
	dist=mt_vec3dot(pos,p)-p[3];

	//Find time of intersection
	t0=(-1-dist)/pvel;
	t1=(1-dist)/pvel;
	if(t0>t1)
	{
		mt_scalar_t tmp=t1;
		t1=t0;
		t0=tmp;
	}

	//Out of bounds (too late or in the past) ?
	if(t0>1||t1<0)
		return 0;

	//Clamp interval to [0..1]
	if(t0<0) t0=0;
	if(t1>1) t1=1;

	pt[0]=pos[0]-p[0]+t0*vel[0];
	pt[1]=pos[1]-p[1]+t0*vel[1];
	pt[2]=pos[2]-p[2]+t0*vel[2];

	//Test each side of the triangle to see if this point lies inside
	mt_vec3cross(e0,p,s);
	mt_vec3diff(pt,v0,d);
	if(mt_vec3dot(s,d)>0)
	{
		mt_vec3cross(e1,p,s);
		mt_vec3diff(pt,v1,d);
		if(mt_vec3dot(s,d)>0)
		{
			mt_vec3cross(e2,p,s);
			mt_vec3diff(pt,v2,d);
			if(mt_vec3dot(s,d)>0)
			{
				//The collision point is inside the triangle
				mt_vec3diff(pt,pos,n);
				*t=t0;
				return 1;
			}
		}
	}

	//Test the edges against the sphere
	MT_SPHERETOEDGE(e0,v1);
	MT_SPHERETOEDGE(e1,v2);
	MT_SPHERETOEDGE(e2,v0);

	//Test the points against the sphere
	c[0]=mt_vec3len2(vel);
	MT_SPHERETOPOINT(v0);
	MT_SPHERETOPOINT(v1);
	MT_SPHERETOPOINT(v2);

	if(found)
	{
		*t=t0;
		return 2;
	}

	return 0;
}
