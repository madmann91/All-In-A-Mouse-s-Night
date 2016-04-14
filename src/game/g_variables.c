#include "g_variables.h"

mt_scalar_t g_gravity;
mt_scalar_t g_friction;
mt_scalar_t g_airfriction;
mt_scalar_t g_step;
mt_scalar_t g_hstep;
mt_scalar_t g_jump;
mt_scalar_t g_distance;
int g_lookat;
mt_vector3_t g_camera[2];
int g_flags;
unsigned int g_delay;

void g_reset(void)
{
	g_gravity=(mt_scalar_t)10.0;
	g_friction=(mt_scalar_t)6.0;
	g_airfriction=(mt_scalar_t)0.1;

	g_step=(mt_scalar_t)1.0;
	g_hstep=(mt_scalar_t)0.1;
	g_jump=(mt_scalar_t)10.0;

	g_distance=(mt_scalar_t)2.0;
	g_lookat=0;
	mt_vec3zero(g_camera[0]);
	mt_vec3zero(g_camera[1]);
	g_camera[1][2]=(mt_scalar_t)10.0;

	g_flags=0;
	g_delay=15;
}
