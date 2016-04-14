#include "config.h"
#include "e_blur.h"
#include "e_camera.h"
#include "e_utils.h"

e_blur_t* e_createblur(e_device_t* device,e_color3_t color,unsigned int count)
{
	e_blur_t* blur=sys_alloc(sizeof(e_blur_t));

	glGenTextures(1,&blur->texture);
	blur->size[0]=e_upow2(device->width);
	blur->size[1]=e_upow2(device->height);

	glBindTexture(GL_TEXTURE_2D,blur->texture);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glCopyTexImage2D(GL_TEXTURE_2D,0,GL_RGB,0,0,blur->size[0],blur->size[1],0);

	blur->count=count;
	blur->color[0]=color[0];
	blur->color[1]=color[1];
	blur->color[2]=color[2];

	return blur;
}

void e_beginblur(e_blur_t* blur,e_device_t* device)
{
	glViewport(0,0,blur->size[0],blur->size[1]);
}

void e_endblur(e_blur_t* blur,e_device_t* device)
{
	unsigned int i;
	float effectcolor[4]={(float)blur->color[0]/255.0f,(float)blur->color[1]/255.0f,(float)blur->color[2]/255.0f,1.0f};
	float inc=0.0f;

	glBindTexture(GL_TEXTURE_2D,blur->texture);
	glCopyTexSubImage2D(GL_TEXTURE_2D,0,0,0,0,0,blur->size[0],blur->size[1]);
	
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();

	glViewport(0,0,device->width,device->height);
	glClear(GL_COLOR_BUFFER_BIT);

	glEnable(GL_TEXTURE_2D);
	glDisable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE);
	glEnable(GL_BLEND);

	glBegin(GL_QUADS);
	for(i=0;i<blur->count;i++)
	{
		glColor4fv(effectcolor);

		glTexCoord2f(inc,inc);
		glVertex2f(-1.0f,-1.0f);

		glTexCoord2f(inc,1.0f-inc);
		glVertex2f(-1.0f,1.0f);

		glTexCoord2f(1.0f-inc,1.0f-inc);
		glVertex2f(1.0f,1.0f);

		glTexCoord2f(1.0f-inc,inc);
		glVertex2f(1.0f,-1.0f);

		inc+=0.1f/(float)(blur->count+1);
		effectcolor[3]-=1.0f/(blur->count+1);
	}
	glEnd();

	glDisable(GL_BLEND);
	glDisable(GL_TEXTURE_2D);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	glPopMatrix();
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}

void e_destroyblur(e_blur_t* blur)
{
	glDeleteTextures(1,&blur->texture);
	sys_free(blur);
}
