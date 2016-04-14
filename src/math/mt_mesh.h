/***********************************************************
*                                                          *
*  File : mt_mesh.h                                        *
*  Date : 13/11/2009                                       *
*  Comments : Triangle mesh routines                       *
*                                                          *
************************************************************/

#ifndef __MT_MESH_H__
#define __MT_MESH_H__

#include "mt_common.h"
#include "mt_vector.h"
#include "mt_transform.h"

/*! \addtogroup mt */
/*! @{ */

/*!
  \name Triangle mesh

  The triangle mesh structure can be used to speed up collision detection with a triangle mesh based model.
  Here's an example of how to use it :

  \code
  unsigned int indices[6]={0,1,2,2,1,3};
  mt_scalar_t vertices[12]={0.0,0.0,0.0,
			1.0,0.0,0.0,
			0.0,1.0,0.0,
			1.0,1.0,0.0};

  mt_mesh_t* mesh;

  mt_vector3_t start={0.0,0.0,0.0};
  mt_vector3_t dir={1.0,1.0,1.0};
  mt_scalar_t t0=1.0;

  mesh=mt_createmesh(indices,6,vertices);
  if(mt_raytomesh(mesh,start,dir,&t0))
  {
     //Do something...
  }
  mt_destroymesh(mesh);
  \endcode
*/
/*! @{ */

struct mt_meshnode_s
{
	unsigned int* indices;
	unsigned int numindices;
	mt_vector3_t min,max;
	unsigned int depth;
	struct mt_meshnode_s* children;
};

typedef struct mt_meshnode_s mt_meshnode_t;

struct mt_mesh_s
{
	mt_meshnode_t* root;
	const mt_scalar_t* vertices;
	unsigned int depth;
};

typedef struct mt_mesh_s mt_mesh_t;

/*!
  \brief Creates a triangle mesh given a list of indices and a list of vertices
  \param[in] indices The list of indices
  \param[in] numindices The number of indices in the list
  \param[in] vertices The list of vertices
  \return The triangle mesh compiled from the vertices
*/
mt_mesh_t* mt_createmesh(unsigned int* indices,unsigned int numindices,const mt_scalar_t* vertices);
/*!
  \brief Refits a triangle mesh given a new set of vertices
  \param[in] mesh The mesh to refit
  \param[in] vertices The new vertices
*/
void mt_refitmesh(mt_mesh_t* mesh,const mt_scalar_t* vertices);
/*!
  \brief Tests a mesh against an axis-aligned box
  \param[in] mesh The triangle mesh to test
  \param[in] position The box position
  \param[in] halfextents The box halfextents
  \return 1 if an intersection was found, otherwise 0
*/
int mt_boxtomesh(const mt_mesh_t* mesh,const mt_vector3_t position,const mt_vector3_t halfextents);
/*!
  \brief Tests a mesh against a ray
  \param[in] mesh The triangle mesh to test
  \param[in] start The ray starting point
  \param[in] direction The ray direction
  \param[in,out] t0 The minimum intersection time
  \return 1 if an intersection was found, otherwise 0
*/
int mt_raytomesh(const mt_mesh_t* mesh,const mt_vector3_t start,const mt_vector3_t direction,mt_scalar_t* t0);
/*!
  \brief Destroys a triangle mesh
  \param[in] mesh The triangle mesh to destroy
*/
void mt_destroymesh(mt_mesh_t* mesh);

/* @} */

/* @} */

#endif

