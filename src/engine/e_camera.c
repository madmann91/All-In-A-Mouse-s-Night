#include "config.h"
#include "e_camera.h"

static mt_matrix4_t e_orthomatrix={(mt_scalar_t)0.0,(mt_scalar_t)0.0,(mt_scalar_t)0.0,(mt_scalar_t)0.0,
									(mt_scalar_t)0.0,(mt_scalar_t)0.0,(mt_scalar_t)0.0,(mt_scalar_t)0.0,
									(mt_scalar_t)0.0,(mt_scalar_t)0.0,(mt_scalar_t)-1.0,(mt_scalar_t)0.0,
									(mt_scalar_t)-1.0,(mt_scalar_t)1.0,(mt_scalar_t)0.0,(mt_scalar_t)1.0};

void e_projectpoint(const e_device_t* device,const e_camera_t* camera,const mt_vector4_t u,mt_vector3_t result)
{
	const mt_scalar_t* model=camera->modelview;
	const mt_scalar_t* proj=camera->projection;
	mt_vector4_t v,w;
	mt_scalar_t inv;
	
	//Rotate and translate point
	v[0]=u[0]*model[0]+u[1]*model[4]+u[2]*model[8]+model[12];
	v[1]=u[0]*model[1]+u[1]*model[5]+u[2]*model[9]+model[13];
	v[2]=u[0]*model[2]+u[1]*model[6]+u[2]*model[10]+model[14];
	v[3]=u[0]*model[3]+u[1]*model[7]+u[2]*model[11]+model[15];

	//Project resulting point
	w[0]=v[0]*proj[0]+v[1]*proj[4]+v[2]*proj[8]+proj[12];
	w[1]=v[0]*proj[1]+v[1]*proj[5]+v[2]*proj[9]+proj[13];
	w[2]=v[0]*proj[2]+v[1]*proj[6]+v[2]*proj[10]+proj[14];
	w[3]=v[0]*proj[3]+v[1]*proj[7]+v[2]*proj[11]+proj[15];

	//Clip it
	inv=(mt_scalar_t)1.0/w[3];
	w[0]*=inv;
	w[1]*=inv;
	w[2]*=inv;

	//Return result
	result[0]=(w[0]+1)*(mt_scalar_t)device->width/2;
	result[1]=(w[1]+1)*(mt_scalar_t)device->height/2;
	result[2]=(w[2]+1)*(mt_scalar_t)0.5;
}

//Some compilers don't like those words...
#undef near
#undef far

void e_setcamera(e_device_t* device,e_camera_t* camera,const mt_vector3_t position,const mt_vector3_t destination,mt_scalar_t far,mt_scalar_t fov)
{
	const mt_scalar_t near=(mt_scalar_t)1.0;
	const mt_scalar_t ratio=(mt_scalar_t)device->width/(mt_scalar_t)device->height;

	mt_scalar_t wnear,wfar,hnear,hfar;
	mt_scalar_t size,proj;
	mt_vector3_t ray,right;
	mt_vector3_t up;

	//Build a ray from position and destination
	ray[0]=destination[0]-position[0];
	ray[1]=destination[1]-position[1];
	ray[2]=destination[2]-position[2];
	mt_vec3norm(ray);

	//Compute the "up" vector
	up[0]=-ray[1]*ray[0];
	up[1]=-ray[1]*ray[1]+(mt_scalar_t)1.0;
	up[2]=-ray[1]*ray[2];
	mt_vec3norm(up);

	//Compute near and far planes
	proj=mt_vec3dot(ray,position);

	camera->frustum[E_PLANE_NEAR][0]=ray[0];
	camera->frustum[E_PLANE_NEAR][1]=ray[1];
	camera->frustum[E_PLANE_NEAR][2]=ray[2];
	camera->frustum[E_PLANE_NEAR][3]=proj+near;

	camera->frustum[E_PLANE_FAR][0]=-ray[0];
	camera->frustum[E_PLANE_FAR][1]=-ray[1];
	camera->frustum[E_PLANE_FAR][2]=-ray[2];
	camera->frustum[E_PLANE_FAR][3]=-proj-far;

	//Compute "right" vector
	mt_vec3cross(ray,up,right);
	mt_vec3norm(right);

	//Compute width and height
	size=mt_tan(fov*(mt_scalar_t)0.5*MT_PI/(mt_scalar_t)180.0);
	hnear=size*near;
	wnear=hnear*ratio;
	hfar=size*far;
	wfar=hfar*ratio;

	//Near plane corners

	//Top-left near corner
	camera->nearcorners[0][0]=position[0]+up[0]*hnear-right[0]*wnear;
	camera->nearcorners[0][1]=position[1]+up[1]*hnear-right[1]*wnear;
	camera->nearcorners[0][2]=position[2]+up[2]*hnear-right[2]*wnear;

	//Top-right near corner
	camera->nearcorners[1][0]=position[0]+up[0]*hnear+right[0]*wnear;
	camera->nearcorners[1][1]=position[1]+up[1]*hnear+right[1]*wnear;
	camera->nearcorners[1][2]=position[2]+up[2]*hnear+right[2]*wnear;

	//Bottom-right near corner
	camera->nearcorners[2][0]=position[0]-up[0]*hnear+right[0]*wnear;
	camera->nearcorners[2][1]=position[1]-up[1]*hnear+right[1]*wnear;
	camera->nearcorners[2][2]=position[2]-up[2]*hnear+right[2]*wnear;

	//Bottom-left near corner
	camera->nearcorners[3][0]=position[0]-up[0]*hnear-right[0]*wnear;
	camera->nearcorners[3][1]=position[1]-up[1]*hnear-right[1]*wnear;
	camera->nearcorners[3][2]=position[2]-up[2]*hnear-right[2]*wnear;

	//Far plane corners

	//Top-left far corner
	camera->farcorners[0][0]=position[0]+up[0]*hfar-right[0]*wfar;
	camera->farcorners[0][1]=position[1]+up[1]*hfar-right[1]*wfar;
	camera->farcorners[0][2]=position[2]+up[2]*hfar-right[2]*wfar;

	//Top-right far corner
	camera->farcorners[1][0]=position[0]+up[0]*hfar+right[0]*wfar;
	camera->farcorners[1][1]=position[1]+up[1]*hfar+right[1]*wfar;
	camera->farcorners[1][2]=position[2]+up[2]*hfar+right[2]*wfar;

	//Bottom-right far corner
	camera->farcorners[2][0]=position[0]-up[0]*hfar+right[0]*wfar;
	camera->farcorners[2][1]=position[1]-up[1]*hfar+right[1]*wfar;
	camera->farcorners[2][2]=position[2]-up[2]*hfar+right[2]*wfar;

	//Bottom-left far corner
	camera->farcorners[3][0]=position[0]-up[0]*hfar-right[0]*wfar;
	camera->farcorners[3][1]=position[1]-up[1]*hfar-right[1]*wfar;
	camera->farcorners[3][2]=position[2]-up[2]*hfar-right[2]*wfar;

	//Compute frustum planes
	mt_triangleplane(camera->nearcorners[0],camera->farcorners[0],camera->farcorners[1],camera->frustum[E_PLANE_TOP]);
	mt_triangleplane(camera->nearcorners[1],camera->farcorners[1],camera->farcorners[2],camera->frustum[E_PLANE_RIGHT]);
	mt_triangleplane(camera->nearcorners[2],camera->farcorners[2],camera->farcorners[3],camera->frustum[E_PLANE_BOTTOM]);
	mt_triangleplane(camera->nearcorners[3],camera->farcorners[3],camera->farcorners[0],camera->frustum[E_PLANE_LEFT]);

	//Build matrices
	glPushMatrix();
	glLoadIdentity();
	gluPerspective(fov,ratio,1.0f,-1.0f);
	glGetFloatv(GL_MODELVIEW_MATRIX,camera->projection);
	glLoadIdentity();
	gluLookAt(position[0],position[1],position[2],
			destination[0],destination[1],destination[2],
			up[0],up[1],up[2]);
	glGetFloatv(GL_MODELVIEW_MATRIX,camera->modelview);
	glPopMatrix();
}

void e_applycamera(e_device_t* device,const e_camera_t* camera)
{
	glMatrixMode(GL_PROJECTION);
#ifndef E_DOUBLE
	glLoadMatrixf(camera->projection);
#else
	glLoadMatrixd(camera->projection);
#endif

	glMatrixMode(GL_MODELVIEW);
#ifndef E_DOUBLE
	glLoadMatrixf(camera->modelview);
#else
	glLoadMatrixd(camera->modelview);
#endif
}

void e_applyortho(e_device_t* device)
{
	e_orthomatrix[0]=(mt_scalar_t)2.0/(mt_scalar_t)device->width;
	e_orthomatrix[5]=(mt_scalar_t)-2.0/(mt_scalar_t)device->height;

	glMatrixMode(GL_PROJECTION);
#ifndef E_DOUBLE
	glLoadMatrixf(e_orthomatrix);
#else
	glLoadMatrixd(e_orthomatrix);
#endif

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}