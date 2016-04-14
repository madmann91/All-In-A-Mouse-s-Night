/***********************************************************
*                                                          *
*  File : e_camera.h                                       *
*  Date : 13/11/2009                                       *
*  Comments : Camera handling                              *
*                                                          *
************************************************************/

#ifndef __E_CAMERA_H__
#define __E_CAMERA_H__

#include "e_device.h"

#include "mt_matrix.h"
#include "mt_plane.h"

/*! \addtogroup e */
/*! @{ */

/*! \name Camera */
/*! @{ */

enum E_CAMERA_PLANES
{
	E_PLANE_NEAR,
	E_PLANE_FAR,
	E_PLANE_LEFT,
	E_PLANE_RIGHT,
	E_PLANE_BOTTOM,
	E_PLANE_TOP
};

/*! Camera structure */
struct e_camera_s
{
	mt_matrix4_t projection;		/*!< Projection matrix */
	mt_matrix4_t modelview;			/*!< Modelview matrix */
	mt_vector3_t nearcorners[4];	/*!< Near plane corners (values : top-left, top-right, bottom-right, bottom-left) */
	mt_vector3_t farcorners[4];		/*!< Far plane corners (values : top-left, top-right, bottom-right, bottom-left) */
	mt_plane_t frustum[6];			/*!< frustum planes (normals pointing inside) */
};

typedef struct e_camera_s e_camera_t;

/*!
  \brief Projects the given point on the screen
  \param[in] device The device conating the viewport information
  \param[in] camera The camera containing the modelview and projection matrices
  \param[in] point The point to project
  \param[in] result The resulting point
*/
void e_projectpoint(const e_device_t* device,const e_camera_t* camera,const mt_vector4_t point,mt_vector3_t result);
/*!
  \brief Sets the camera parameters
  \param[in] device The device the camera belongs to
  \param[in] camera The camera to set
  \param[in] position The eye position
  \param[in] destination The point to look at
  \param[in] distance The view distance
  \param[in] fov The field of view, in degrees
*/
void e_setcamera(e_device_t* device,e_camera_t* camera,const mt_vector3_t position,const mt_vector3_t destination,mt_scalar_t distance,mt_scalar_t fov);
/*!
  \brief Changes the device camera
  \param[in] device The device
  \param[in] camera The new device camera
*/
void e_applycamera(e_device_t* device,const e_camera_t* camera);
/*!
  \brief Sets an orthographic projection matrix
  \param[in] device The device
*/
void e_applyortho(e_device_t* device);

/*! @} */

/*! @} */

#endif
