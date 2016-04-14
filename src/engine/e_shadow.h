/***********************************************************
*                                                          *
*  File : e_shadow.h                                       *
*  Date : 13/11/2009                                       *
*  Comments : Useful shadow-related functions              *
*                                                          *
************************************************************/

#ifndef __E_SHADOW_H__
#define __E_SHADOW_H__

#include "e_common.h"
#include "e_device.h"
#include "e_camera.h"

#include "mt_vector.h"
#include "mt_plane.h"

/*! \addtogroup e */
/*! @{ */

/*! \name Shadow */
/*! @{ */

#define E_OUTLINES_WIDTH 3.0f
//#define E_OUTLINES_SMOOTH

enum E_RENDER_PASSES
{
	E_RENDER_MESHES,		/*!< 1st render pass : fill depth buffer and give base illumination */
	E_RENDER_SHADOWS,		/*!< 2nd render pass : render shadow volumes */
	E_RENDER_SHADING,		/*!< 3rd render pass : render scene with shading enabled */
	E_RENDER_OUTLINES		/*!< 4th render pass : render outlines */
};

/*! A triangle edge */
struct e_edge_s
{
	unsigned int t0,t1;			/*!< The triangle indices that make up this edge */
	unsigned int v0,v1;			/*!< The vertex indices */
};

typedef struct e_edge_s e_edge_t;

struct e_lightsettings_s
{
	mt_vector4_t attenuation;	/*!< Attenuation factors */
	mt_vector3_t position;		/*!< Light position */
	e_color3_t color;			/*!< Light color */
	mt_scalar_t radius;			/*!< Light radius (computed from attenuation factors) */
	mt_plane_t planes[6];		/*!< Light planes (computed from the camera) */			
	int scissor[4];				/*!< Scissor box (computed from the radius and camera) */
};

typedef struct e_lightsettings_s e_lightsettings_t;

/*!
  \brief Computes the light settings
  \param[in] camera The camera object with which the scene is to be rendered
  \param[in] limit The limit above which the attenuation will give no lighting (user value)
  \param[in,out] settings The light settings
  The position, attenuation, and color fields of "settings" must have been previously set.
*/
void e_setuplight(const e_camera_t* camera,mt_scalar_t limit,e_lightsettings_t* settings);

/*!
  \brief Computes the shadow volume caps indices
  \param[in,out] volume The shadow volume indices
  \param[in] facing The per-triangle light-facing information
  \param[in] indices The model indices
  \param[in] numindices The number of indices
*/
unsigned int e_createcaps(unsigned int* volume,const unsigned char* facing,const unsigned int* indices,unsigned int numindices);
/*!
  \brief Computes the shadow volume caps indices
  \param[in,out] volume The shadow volume indices
  \param[in] facing The per-triangle light-facing information
  \param[in] edges The model edges
  \param[in] numedges The number of edges
*/
unsigned int e_createsilhouette(unsigned int* volume,const unsigned char* facing,const e_edge_t* edges,unsigned int numedges);

/*! \brief Prepares another shadow pass */
void e_clearshadows(e_device_t* device);

/*! \brief Starts a render pass */
void e_beginpass(e_device_t* device,unsigned int pass);
/*! \brief Ends a render pass */
void e_endpass(e_device_t* device,unsigned int pass);

/*! @} */

/*! @} */

#endif
