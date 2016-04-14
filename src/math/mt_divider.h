/***********************************************************
*                                                          *
*  File : mt_divider.h                                     *
*  Date : 20/02/2011                                       *
*  Comments : Spacial subdivision structure                *
*                                                          *
************************************************************/

#ifndef __MT_DIVIDER_H__
#define __MT_DIVIDER_H__

#include "mt_common.h"
#include "mt_vector.h"
#include "mt_plane.h"

/*! \addtogroup mt */
/*! @{ */

/*! \name Divider */
/*! @{ */

/*! A divider node structure */
struct mt_dividernode_s
{
	mt_vector3_t min;					/*!< The minimum bounding box dimensions */
	mt_vector3_t max;					/*!< The maximum bounding box dimensions */
	unsigned int* colliders;			/*!< The colliders index array, if this node is a leaf */
	unsigned int numcolliders;			/*!< The number of colliders in this node */
	unsigned int maxcolliders;			/*!< The size of the colliders array */
	struct mt_dividernode_s* children;	/*!< The children of this node */
};

typedef struct mt_dividernode_s mt_dividernode_t;

/*! A collider object */
struct mt_collider_s
{
	mt_dividernode_t* node;	/*!< The node this collider belongs to */
	mt_vector3_t min;		/*!< The enclosing bounding box minimum point */
	mt_vector3_t max;		/*!< The enclosing bounding box maximum point */
	void* user;				/*!< The user data associated with the collider */
};

typedef struct mt_collider_s mt_collider_t;

/*! The divider structure */
struct mt_divider_s
{
	mt_collider_t* colliders;	/*!< The collider array */
	unsigned int numcolliders;	/*!< The number of colliders */
	mt_dividernode_t* root;		/*!< Root node */
	unsigned int depth;			/*!< The tree depth */
	unsigned int* query;		/*!< The query table */
	unsigned int numquery;		/*!< The number of elements in the query table */
	unsigned int maxquery;		/*!< The size of the query table */
};

typedef struct mt_divider_s mt_divider_t;

/*
  \brief Creates a divider object
  \param[in] numcolliders The number of colliders to allocate
  \param[in] depth The maximum tree depth
  \param[in] min The minimum dimensions of the world
  \param[in] max The maximum dimensions of the world
  \return A valid divider structure, or 0 if an error occured
*/
mt_divider_t* mt_createdivider(unsigned int depth,const mt_vector3_t min,const mt_vector3_t max);
/*
  \brief Resizes a divider object
  \param[in] divider The divider to be resized
  \param[in] numcolliders The number of colliders to (re)allocate
*/
void mt_resizedivider(mt_divider_t* divider,unsigned int numcolliders);
/*
  \brief Updates a collider in the divider superstructure
  \param[in] divider The divider whose specified collider is to be updated
  \param[in] index The index of the collider
  \param[in] min The new minimum dimensions
  \param[in] max The new maximum dimensions
*/
void mt_updatecollider(mt_divider_t* divider,unsigned int index,const mt_vector3_t min,const mt_vector3_t max);
/*
  \brief Returns the colliders hit by the given ray
  \param[in] divider The divider structure containing the colliders
  \param[out] count The number of colliders found
  \param[in] start The starting point of the ray
  \param[in] direction The ray direction (scaled so that the time of intersection is in [0..1])
  \return The array of collider indices
*/
const unsigned int* mt_rayquery(mt_divider_t* divider,unsigned int* count,const mt_vector3_t start,const mt_vector3_t direction);
/*
  \brief Returns the colliders colliding with the given box
  \param[in] divider The divider structure containing the colliders
  \param[out] count The number of colliders found
  \param[in] min The minimum dimensions of the box
  \param[in] max The maximum dimensions of the box
  \return The array of collider indices
*/
const unsigned int* mt_boxquery(mt_divider_t* divider,unsigned int* count,const mt_vector3_t min,const mt_vector3_t max);
/*
  \brief Returns the colliders colliding with the given frustum
  \param[in] divider The divider structure containing the colliders
  \param[out] count The number of colliders found
  \param[in] planes The planes that define the frustum
  \return The array of collider indices
*/
const unsigned int* mt_frustumquery(mt_divider_t* divider,unsigned int* count,const mt_plane_t planes[6]);
/*
  \brief Destroys a divider object
  \param[in] divider The divider to be destroyed
*/
void mt_destroydivider(mt_divider_t* divider);

/*! @} */

/*! @} */

#endif

