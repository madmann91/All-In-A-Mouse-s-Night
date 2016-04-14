#include "config.h"
#include "mt_matrix.h"

void mt_mat3copy(mt_matrix3_t a,const mt_matrix3_t b)
{
	a[0]=b[0];
	a[1]=b[1];
	a[2]=b[2];
	a[3]=b[3];
	a[4]=b[4];
	a[5]=b[5];
	a[6]=b[6];
	a[7]=b[7];
	a[8]=b[8];
}

void mt_mat3id(mt_matrix3_t m)
{
	m[0]=(mt_scalar_t)1.0;
	m[1]=(mt_scalar_t)0.0;
	m[2]=(mt_scalar_t)0.0;

	m[3]=(mt_scalar_t)0.0;
	m[4]=(mt_scalar_t)1.0;
	m[5]=(mt_scalar_t)0.0;

	m[6]=(mt_scalar_t)0.0;
	m[7]=(mt_scalar_t)0.0;
	m[8]=(mt_scalar_t)1.0;
}

void mt_mat3rot(mt_matrix3_t m,mt_scalar_t x,mt_scalar_t y,mt_scalar_t z)
{
	const mt_scalar_t ci=mt_cos(x);
	const mt_scalar_t cj=mt_cos(y);
	const mt_scalar_t ch=mt_cos(z);
	const mt_scalar_t si=mt_sin(x);
	const mt_scalar_t sj=mt_sin(y);
	const mt_scalar_t sh=mt_sin(z);
	const mt_scalar_t cc=ci*ch;
	const mt_scalar_t cs=ci*sh;
	const mt_scalar_t sc=si*ch;
	const mt_scalar_t ss=si*sh;

	m[0]=cj*ch;
	m[1]=sj*sc-cs;
	m[2]=sj*cc+ss;

	m[3]=cj*sh;
	m[4]=sj*ss+cc;
	m[5]=sj*cs-sc;

	m[6]=-sj;
	m[7]=cj*si;
	m[8]=cj*ci;
}

void mt_mat3yrot(mt_matrix3_t m,mt_scalar_t y)
{
	m[0]=mt_cos(y);
	m[2]=mt_sin(y);
	m[6]=-m[2];
	m[8]=m[0];

	m[1]=m[3]=m[5]=m[7]=(mt_scalar_t)0.0;
	m[4]=(mt_scalar_t)1.0;
}

void mt_mat3transp(mt_matrix3_t a,const mt_matrix3_t b)
{
	a[0]=b[0];
	a[1]=b[3];
	a[2]=b[6];

	a[3]=b[1];
	a[4]=b[4];
	a[5]=b[7];

	a[6]=b[2];
	a[7]=b[5];
	a[8]=b[8];
}

void mt_mat3abs(mt_matrix3_t a,const mt_matrix3_t b)
{
	a[0]=mt_fabs(b[0]);
	a[1]=mt_fabs(b[1]);
	a[2]=mt_fabs(b[2]);

	a[3]=mt_fabs(b[3]);
	a[4]=mt_fabs(b[4]);
	a[5]=mt_fabs(b[5]);

	a[6]=mt_fabs(b[6]);
	a[7]=mt_fabs(b[7]);
	a[8]=mt_fabs(b[8]);
}

mt_scalar_t mt_mat3cofac(const mt_matrix3_t m,unsigned int r1,unsigned int c1,unsigned int r2,unsigned int c2)
{
	return m[r1*3+c1]*m[r2*3+c2]-m[r1*3+c2]*m[r2*3+c1];
}

void mt_mat3inv(mt_matrix3_t a,const mt_matrix3_t b)
{
	const mt_vector3_t cf={mt_mat3cofac(b,1,1,2,2),mt_mat3cofac(b,1,2,2,0),mt_mat3cofac(b,1,0,2,1)};
	const mt_scalar_t det=cf[0]*b[0]+cf[1]*b[1]+cf[2]*b[2];
	const mt_scalar_t inv=1.0f/det;

	a[0]=cf[0]*inv;
	a[1]=mt_mat3cofac(b,0,2,2,1)*inv;
	a[2]=mt_mat3cofac(b,0,1,1,2)*inv;

	a[3]=cf[1]*inv;
	a[4]=mt_mat3cofac(b,0,0,2,2)*inv;
	a[5]=mt_mat3cofac(b,0,2,1,0)*inv;

	a[6]=cf[2]*inv;
	a[7]=mt_mat3cofac(b,0,1,2,0)*inv;
	a[8]=mt_mat3cofac(b,0,0,1,1)*inv;
}

void mt_mat3mulmat3(mt_matrix3_t m,const mt_matrix3_t a,const mt_matrix3_t b)
{
	m[0]=a[0]*b[0]+a[1]*b[3]+a[2]*b[6];
	m[1]=a[0]*b[1]+a[1]*b[4]+a[2]*b[7];
	m[2]=a[0]*b[2]+a[1]*b[5]+a[2]*b[8];
	m[3]=a[3]*b[0]+a[4]*b[3]+a[5]*b[6];
	m[4]=a[3]*b[1]+a[4]*b[4]+a[5]*b[7];
	m[5]=a[3]*b[2]+a[4]*b[5]+a[5]*b[8];
	m[6]=a[6]*b[0]+a[7]*b[3]+a[8]*b[6];
	m[7]=a[6]*b[1]+a[7]*b[4]+a[8]*b[7];
	m[8]=a[6]*b[2]+a[7]*b[5]+a[8]*b[8];
}

void mt_mat3mulvec3(mt_vector3_t v,const mt_matrix3_t m,const mt_vector3_t u)
{
	v[0]=u[0]*m[0]+u[1]*m[1]+u[2]*m[2];
	v[1]=u[0]*m[3]+u[1]*m[4]+u[2]*m[5];
	v[2]=u[0]*m[6]+u[1]*m[7]+u[2]*m[8];
}

void mt_mat3mulvec4(mt_vector4_t v,const mt_matrix3_t m,const mt_vector4_t u)
{
	v[0]=u[0]*m[0]+u[1]*m[1]+u[2]*m[2];
	v[1]=u[0]*m[3]+u[1]*m[4]+u[2]*m[5];
	v[2]=u[0]*m[6]+u[1]*m[7]+u[2]*m[8];
	v[3]=(mt_scalar_t)1.0;
}

void mt_mat4copy(mt_matrix4_t a,const mt_matrix4_t b)
{
	a[0]=b[0];
	a[1]=b[1];
	a[2]=b[2];
	a[3]=b[3];

	a[4]=b[4];
	a[5]=b[5];
	a[6]=b[6];
	a[7]=b[7];

	a[8]=b[8];
	a[9]=b[9];
	a[10]=b[10];
	a[11]=b[11];

	a[12]=b[12];
	a[13]=b[13];
	a[14]=b[14];
	a[15]=b[15];
}

void mt_mat4id(mt_matrix4_t m)
{
	m[0]=(mt_scalar_t)1.0;
	m[1]=(mt_scalar_t)0.0;
	m[2]=(mt_scalar_t)0.0;
	m[3]=(mt_scalar_t)0.0;

	m[4]=(mt_scalar_t)0.0;
	m[5]=(mt_scalar_t)1.0;
	m[6]=(mt_scalar_t)0.0;
	m[7]=(mt_scalar_t)0.0;

	m[8]=(mt_scalar_t)0.0;
	m[9]=(mt_scalar_t)0.0;
	m[10]=(mt_scalar_t)1.0;
	m[11]=(mt_scalar_t)0.0;

	m[12]=(mt_scalar_t)0.0;
	m[13]=(mt_scalar_t)0.0;
	m[14]=(mt_scalar_t)0.0;
	m[15]=(mt_scalar_t)1.0;
}

void mt_mat4transp(mt_matrix4_t a,const mt_matrix4_t b)
{
	a[0]=b[0];
	a[1]=b[4];
	a[2]=b[8];
	a[3]=b[12];

	a[4]=b[1];
	a[5]=b[5];
	a[6]=b[9];
	a[7]=b[13];

	a[8]=b[2];
	a[9]=b[6];
	a[10]=b[10];
	a[11]=b[14];

	a[12]=b[3];
	a[13]=b[7];
	a[14]=b[11];
	a[15]=b[15];
}

void mt_mat4mulmat4(mt_matrix4_t c,const mt_matrix4_t a,const mt_matrix4_t b)
{
	c[0]=a[0]*b[0]+a[4]*b[1]+a[8]*b[2]+a[12]*b[3];
	c[1]=a[1]*b[0]+a[5]*b[1]+a[9]*b[2]+a[13]*b[3];
	c[2]=a[2]*b[0]+a[6]*b[1]+a[10]*b[2]+a[14]*b[3];
	c[3]=a[3]*b[0]+a[7]*b[1]+a[11]*b[2]+a[15]*b[3];

	c[4]=a[0]*b[4]+a[4]*b[5]+a[8]*b[6]+a[12]*b[7];
	c[5]=a[1]*b[4]+a[5]*b[5]+a[9]*b[6]+a[13]*b[7];
	c[6]=a[2]*b[4]+a[6]*b[5]+a[10]*b[6]+a[14]*b[7];
	c[7]=a[3]*b[4]+a[7]*b[5]+a[11]*b[6]+a[15]*b[7];

	c[8]=a[0]*b[8]+a[4]*b[9]+a[8]*b[10]+a[12]*b[11];
	c[9]=a[1]*b[8]+a[5]*b[9]+a[9]*b[10]+a[13]*b[11];
	c[10]=a[2]*b[8]+a[6]*b[9]+a[10]*b[10]+a[14]*b[11];
	c[11]=a[3]*b[8]+a[7]*b[9]+a[11]*b[10]+a[15]*b[11];

	c[12]=a[0]*b[12]+a[4]*b[13]+a[8]*b[14]+a[12]*b[15];
	c[13]=a[1]*b[12]+a[5]*b[13]+a[9]*b[14]+a[13]*b[15];
	c[14]=a[2]*b[12]+a[6]*b[13]+a[10]*b[14]+a[14]*b[15];
	c[15]=a[3]*b[12]+a[7]*b[13]+a[11]*b[14]+a[15]*b[15];
}

void mt_mat4rmulmat4(mt_matrix4_t c,const mt_matrix4_t a,const mt_matrix4_t b)
{
	c[0]=a[0]*b[0]+a[4]*b[1]+a[8]*b[2];
	c[1]=a[1]*b[0]+a[5]*b[1]+a[9]*b[2];
	c[2]=a[2]*b[0]+a[6]*b[1]+a[10]*b[2];
	c[3]=(mt_scalar_t)0.0;

	c[4]=a[0]*b[4]+a[4]*b[5]+a[8]*b[6];
	c[5]=a[1]*b[4]+a[5]*b[5]+a[9]*b[6];
	c[6]=a[2]*b[4]+a[6]*b[5]+a[10]*b[6];
	c[7]=(mt_scalar_t)0.0;

	c[8]=a[0]*b[8]+a[4]*b[9]+a[8]*b[10];
	c[9]=a[1]*b[8]+a[5]*b[9]+a[9]*b[10];
	c[10]=a[2]*b[8]+a[6]*b[9]+a[10]*b[10];
	c[11]=(mt_scalar_t)0.0;

	c[12]=a[0]*b[12]+a[4]*b[13]+a[8]*b[14]+a[12];
	c[13]=a[1]*b[12]+a[5]*b[13]+a[9]*b[14]+a[13];
	c[14]=a[2]*b[12]+a[6]*b[13]+a[10]*b[14]+a[14];
	c[15]=(mt_scalar_t)1.0;
}

void mt_mat4mulvec3(mt_vector3_t u,const mt_matrix4_t m,const mt_vector3_t v)
{
	u[0]=v[0]*m[0]+v[1]*m[4]+v[2]*m[8]+m[12];
	u[1]=v[0]*m[1]+v[1]*m[5]+v[2]*m[9]+m[13];
	u[2]=v[0]*m[2]+v[1]*m[6]+v[2]*m[10]+m[14];
}

void mt_mat4mulvec4(mt_vector4_t u,const mt_matrix4_t m,const mt_vector4_t v)
{
	u[0]=v[0]*m[0]+v[1]*m[4]+v[2]*m[8]+m[12]*v[3];
	u[1]=v[0]*m[1]+v[1]*m[5]+v[2]*m[9]+m[13]*v[3];
	u[2]=v[0]*m[2]+v[1]*m[6]+v[2]*m[10]+m[14]*v[3];
	u[3]=v[0]*m[3]+v[1]*m[7]+v[2]*m[11]+m[15]*v[3];
}

void mt_mat4rmulvec3(mt_vector3_t u,const mt_matrix4_t m,const mt_vector3_t v)
{
	u[0]=v[0]*m[0]+v[1]*m[4]+v[2]*m[8];
	u[1]=v[0]*m[1]+v[1]*m[5]+v[2]*m[9];
	u[2]=v[0]*m[2]+v[1]*m[6]+v[2]*m[10];
}

void mt_mat4rmulvec4(mt_vector4_t u,const mt_matrix4_t m,const mt_vector4_t v)
{
	u[0]=v[0]*m[0]+v[1]*m[4]+v[2]*m[8];
	u[1]=v[0]*m[1]+v[1]*m[5]+v[2]*m[9];
	u[2]=v[0]*m[2]+v[1]*m[6]+v[2]*m[10];
	u[3]=(mt_scalar_t)1.0;
}

void mt_mat4orthoinv(mt_matrix4_t a,const mt_matrix4_t b)
{
	a[0]=b[0];
	a[1]=b[4];
	a[2]=b[8];
	a[3]=(mt_scalar_t)0.0;

	a[4]=b[1];
	a[5]=b[5];
	a[6]=b[9];
	a[7]=(mt_scalar_t)0.0;

	a[8]=b[2];
	a[9]=b[6];
	a[10]=b[10];
	a[11]=(mt_scalar_t)0.0;

	a[12]=-(b[0]*b[12]+b[1]*b[13]+b[2]*b[14]);
	a[13]=-(b[4]*b[12]+b[5]*b[13]+b[6]*b[14]);
	a[14]=-(b[8]*b[12]+b[9]*b[13]+b[10]*b[14]);
	a[15]=(mt_scalar_t)1.0;
}
