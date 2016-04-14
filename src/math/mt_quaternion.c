#include "config.h"
#include "mt_quaternion.h"

void mt_quatset(mt_quaternion_t q,mt_scalar_t x,mt_scalar_t y,mt_scalar_t z,mt_scalar_t w)
{
	q[0]=x;
	q[1]=y;
	q[2]=z;
	q[3]=w;
}

void mt_quatcopy(mt_quaternion_t a,const mt_quaternion_t b)
{
	a[0]=b[0];
	a[1]=b[1];
	a[2]=b[2];
	a[3]=b[3];
}

void mt_quatzero(mt_quaternion_t q)
{
	q[0]=0;
	q[1]=0;
	q[2]=0;
	q[3]=0;
}

void mt_quatnorm(mt_quaternion_t q)
{
	const mt_scalar_t d=(mt_scalar_t)1.0/mt_quatlen(q);
	q[0]*=d;
	q[1]*=d;
	q[2]*=d;
	q[3]*=d;
}

mt_scalar_t mt_quatlen(const mt_quaternion_t q)
{
	return mt_sqrt(q[0]*q[0]+q[1]*q[1]+q[2]*q[2]+q[3]*q[3]);
}

mt_scalar_t mt_quatlen2(const mt_quaternion_t q)
{
	return q[0]*q[0]+q[1]*q[1]+q[2]*q[2]+q[3]*q[3];
}

void mt_quatinv(mt_quaternion_t a,const mt_quaternion_t b)
{
	a[0]=-b[0];
	a[1]=-b[1];
	a[2]=-b[2];
	a[3]=b[3];
}

void mt_quatori(mt_vector3_t v,const mt_quaternion_t q)
{
	//Rotates (1,0,0) with q
	v[0]=-q[3]*q[3]-q[0]*q[0]+q[1]*q[1]+q[2]*q[2];
	v[1]=-(mt_scalar_t)2.0*(q[3]*q[2]+q[0]*q[1]);
	v[2]=(mt_scalar_t)2.0*(q[3]*q[1]-q[0]*q[2]);
}

void mt_quatrot(mt_quaternion_t q,const mt_vector3_t angles)
{
	mt_scalar_t hx=angles[0]*(mt_scalar_t)0.5;  
	mt_scalar_t hy=angles[1]*(mt_scalar_t)0.5;  
	mt_scalar_t hz=angles[2]*(mt_scalar_t)0.5;  
	mt_scalar_t cx=mt_cos(hx);
	mt_scalar_t sx=mt_sin(hx);
	mt_scalar_t cy=mt_cos(hy);
	mt_scalar_t sy=mt_sin(hy);
	mt_scalar_t cz=mt_cos(hz);
	mt_scalar_t sz=mt_sin(hz);

	q[0]=cz*sy*cx+sz*cy*sx;
	q[1]=cz*cy*sx-sz*sy*cx;
	q[2]=sz*cy*cx-cz*sy*sx;
	q[3]=cz*cy*cx+sz*sy*sx;
}

void mt_quatslerp(mt_quaternion_t c,const mt_quaternion_t a,const mt_quaternion_t b,mt_scalar_t t)
{
	mt_scalar_t dot=a[0]*b[0]+a[1]*b[1]+a[2]*b[2]+a[3]*b[3];
	mt_scalar_t angle,inv,f0,f1;

	if(mt_fabs(dot)>=(mt_scalar_t)1.0)
	{
		f0=1.0f-t;
		c[0]=f0*a[0]+t*b[0];
		c[1]=f0*a[1]+t*b[1];
		c[2]=f0*a[2]+t*b[2];
		c[3]=f0*a[3]+t*b[3];
	}
	else
	{
		angle=mt_acos(dot);
		inv=(mt_scalar_t)1.0/mt_sin(angle);
		f0=mt_sin(((mt_scalar_t)1.0-t)*angle)*inv;
		f1=mt_sin(t*angle)*inv;

		if(dot>(mt_scalar_t)0.0)
		{
			c[0]=f0*a[0]+f1*b[0];
			c[1]=f0*a[1]+f1*b[1];
			c[2]=f0*a[2]+f1*b[2];
			c[3]=f0*a[3]+f1*b[3];
		}
		else
		{
			c[0]=f0*a[0]-f1*b[0];
			c[1]=f0*a[1]-f1*b[1];
			c[2]=f0*a[2]-f1*b[2];
			c[3]=f0*a[3]-f1*b[3];
		}
	}
}

void mt_quatnlerp(mt_quaternion_t c,const mt_quaternion_t a,const mt_quaternion_t b,mt_scalar_t t)
{
	mt_scalar_t dot=a[0]*b[0]+a[1]*b[1]+a[2]*b[2]+a[3]*b[3];
	const mt_scalar_t f=(mt_scalar_t)1.0-t;
	mt_scalar_t inv;

	if(dot>(mt_scalar_t)0.0)
	{
		c[0]=f*a[0]+t*b[0];
		c[1]=f*a[1]+t*b[1];
		c[2]=f*a[2]+t*b[2];
		c[3]=f*a[3]+t*b[3];
	}
	else
	{
		c[0]=f*a[0]-t*b[0];
		c[1]=f*a[1]-t*b[1];
		c[2]=f*a[2]-t*b[2];
		c[3]=f*a[3]-t*b[3];
	}

	inv=(mt_scalar_t)1.0/mt_quatlen(c);

	c[0]*=inv;
	c[1]*=inv;
	c[2]*=inv;
	c[3]*=inv;
}

void mt_quatscale(mt_quaternion_t q,mt_scalar_t s)
{
	q[0]*=s;
	q[1]*=s;
	q[2]*=s;
	q[3]*=s;
}

void mt_quatmulquat(mt_quaternion_t c,const mt_quaternion_t a,const mt_quaternion_t b)
{
	c[0]=a[3]*b[0]+a[0]*b[3]+a[1]*b[2]-a[2]*b[1];
	c[1]=a[3]*b[1]-a[0]*b[2]+a[1]*b[3]+a[2]*b[0];
	c[2]=a[3]*b[2]+a[0]*b[1]-a[1]*b[0]+a[2]*b[3];
	c[3]=a[3]*b[3]-a[0]*b[0]-a[1]*b[1]-a[2]*b[2];
}

void mt_quatimulquat(mt_quaternion_t c,const mt_quaternion_t a,const mt_quaternion_t b)
{
	c[0]=a[3]*b[0]-a[0]*b[3]-a[1]*b[2]+a[2]*b[1];
	c[1]=a[3]*b[1]+a[0]*b[2]-a[1]*b[3]-a[2]*b[0];
	c[2]=a[3]*b[2]-a[0]*b[1]+a[1]*b[0]-a[2]*b[3];
	c[3]=a[3]*b[3]+a[0]*b[0]+a[1]*b[1]+a[2]*b[2];
}

void mt_quatmulvec3(mt_vector3_t v,const mt_quaternion_t q,const mt_vector3_t u)
{
	const mt_quaternion_t r={-(q[3]*u[0]+q[1]*u[2]-q[2]*u[1]),
		-(q[3]*u[1]-q[0]*u[2]+q[2]*u[0]),
		-(q[3]*u[2]+q[0]*u[1]-q[1]*u[0]),
		-q[0]*u[0]-q[1]*u[1]-q[2]*u[2]};

	v[0]=q[3]*r[0]+q[0]*r[3]+q[1]*r[2]-q[2]*r[1];
	v[1]=q[3]*r[1]-q[0]*r[2]+q[1]*r[3]+q[2]*r[0];
	v[2]=q[3]*r[2]+q[0]*r[1]-q[1]*r[0]+q[2]*r[3];
}

void mt_quatimulvec3(mt_vector3_t v,const mt_quaternion_t q,const mt_vector3_t u)
{
	const mt_quaternion_t r={-(q[3]*u[0]-q[1]*u[2]+q[2]*u[1]),
		-(q[3]*u[1]+q[0]*u[2]-q[2]*u[0]),
		-(q[3]*u[2]-q[0]*u[1]+q[1]*u[0]),
		q[0]*u[0]+q[1]*u[1]+q[2]*u[2]};

	v[0]=q[3]*r[0]-q[0]*r[3]-q[1]*r[2]+q[2]*r[1];
	v[1]=q[3]*r[1]+q[0]*r[2]-q[1]*r[3]-q[2]*r[0];
	v[2]=q[3]*r[2]-q[0]*r[1]+q[1]*r[0]-q[2]*r[3];
}

mt_scalar_t mt_quatdot(const mt_quaternion_t a,const mt_quaternion_t b)
{
	return a[0]*b[0]+a[1]*b[1]+a[2]*b[2]+a[3]*b[3];
}

void mt_quattomat3(mt_matrix3_t m,const mt_quaternion_t q)
{
	const mt_scalar_t d=q[0]*q[0]+q[1]*q[1]+q[2]*q[2]+q[3]*q[3];
	const mt_scalar_t s=(mt_scalar_t)2.0/d;

	const mt_vector3_t u={q[0]*s,q[1]*s,q[2]*s};
	const mt_vector3_t v={q[1]*u[1],q[1]*u[2],q[2]*u[2]};

	const mt_vector3_t w={q[3]*u[0],q[3]*u[1],q[3]*u[2]};
	const mt_vector3_t x={q[0]*u[0],q[0]*u[1],q[0]*u[2]};

	m[0]=(mt_scalar_t)1.0-(v[0]+v[2]);
	m[1]=x[1]-w[2];
	m[2]=x[2]+w[1];

	m[3]=x[1]+w[2];
	m[4]=(mt_scalar_t)1.0-(x[0]+v[2]);
	m[5]=v[1]-w[0];

	m[6]=x[2]-w[1];
	m[7]=v[1]+w[0];
	m[8]=(mt_scalar_t)1.0-(x[0]+v[0]);
}

void mt_quattomat4(mt_matrix4_t m,const mt_quaternion_t q,const mt_vector3_t p)
{
	const mt_scalar_t d=q[0]*q[0]+q[1]*q[1]+q[2]*q[2]+q[3]*q[3];
	const mt_scalar_t s=(mt_scalar_t)2.0/d;

	const mt_vector3_t u={q[0]*s,q[1]*s,q[2]*s};
	const mt_vector3_t v={q[1]*u[1],q[1]*u[2],q[2]*u[2]};

	const mt_vector3_t w={q[3]*u[0],q[3]*u[1],q[3]*u[2]};
	const mt_vector3_t x={q[0]*u[0],q[0]*u[1],q[0]*u[2]};

	m[0]=(mt_scalar_t)1.0-(v[0]+v[2]);
	m[1]=x[1]+w[2];
	m[2]=x[2]-w[1];
	m[3]=(mt_scalar_t)0.0;

	m[4]=x[1]-w[2];
	m[5]=(mt_scalar_t)1.0-(x[0]+v[2]);
	m[6]=v[1]+w[0];
	m[7]=(mt_scalar_t)0.0;

	m[8]=x[2]+w[1];
	m[9]=v[1]-w[0];
	m[10]=(mt_scalar_t)1.0-(x[0]+v[0]);
	m[11]=(mt_scalar_t)0.0;

	m[12]=p[0];
	m[13]=p[1];
	m[14]=p[2];
	m[15]=(mt_scalar_t)1.0;
}
