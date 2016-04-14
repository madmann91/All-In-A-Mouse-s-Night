/***********************************************************
*                                                          *
*  File : e_skeleton.h                                     *
*  Date : 13/11/2009                                       *
*  Comments : Skeleton computations                        *
*                                                          *
************************************************************/

#ifndef __E_SKELETON_H__
#define __E_SKELETON_H__

#include "e_common.h"
#include "e_device.h"

#include "mt_vector.h"
#include "mt_quaternion.h"

#include "fs_common.h"

/*! \addtogroup e */
/*! @{ */

/*! \name Skeleton */
/*! @{ */

extern const e_filetype_t e_skeletonfile;

/*! An animation frame */
struct e_frame_s
{
	mt_quaternion_t quat;		/*!< Orientation defined by a quaternion */
	mt_vector3_t pos;			/*!< Position vector */
	mt_scalar_t len;			/*!< Scale factor */
};

typedef struct e_frame_s e_frame_t;

/*! A bone action */
struct e_action_s
{
	unsigned int begin;			/*!< Starting frame */
	unsigned int end;			/*!< Ending frame */
	mt_scalar_t speed;			/*!< Action speed */
};

typedef struct e_action_s e_action_t;

struct e_animation_s
{
	int blend:1;				/*!< Blending flag */
	mt_scalar_t lambda;			/*!< Blending factor */

	unsigned int action1;		/*!< The first action */
	unsigned int anim1[2];		/*!< The first animation frames */
	mt_scalar_t factor1;		/*!< Interpolation factor for the first animation */

	unsigned int action2;		/*!< The second action */
	unsigned int anim2[2];		/*!< The second animation frames */
	mt_scalar_t factor2;		/*!< Interpolation factor for the second animation */
};

typedef struct e_animation_s e_animation_t;

/*! A skeleton buffer */
struct e_skeletonbuffer_s
{
	e_frame_t* state;		/*!< The computed frame (relative to the base frame) */
	mt_matrix4_t* current;	/*!< The computed frame */
};

typedef struct e_skeletonbuffer_s e_skeletonbuffer_t;

/*! Skeleton structure */
struct e_skeleton_s
{
	unsigned int numbones;		/*!< Number of bones */
	int* bones;					/*!< Skeleton bones */
	unsigned int numframes;		/*!< Number of frames */
	e_frame_t** frames;			/*!< Skeleton frames (0 is the base frame) */
	unsigned int numactions;	/*!< Number of actions */
	e_action_t* actions;		/*!< Skeleton actions */
	unsigned int hands[2];		/*!< The indices of the hand bones (left,right) */
	e_frame_t* base;			/*!< The inverse of the base frame */
};

typedef struct e_skeleton_s e_skeleton_t;

/*!
  \brief Creates a skeleton buffer
  \param[in] skeleton The skeleton to associate with the buffer
  \return A valid skeleton buffer, or 0 if an error occured
*/
e_skeletonbuffer_t* e_createskeletonbuffer(e_skeleton_t* skeleton);
/*!
  \brief Updates a skeleton buffer
  \param[in] buffer The skeleton buffer that will receive the computed frames
  \param[in] skeleton The skeleton containing frames information
  \param[in] anim The skeleton animation information
*/
void e_updateskeletonbuffer(e_skeletonbuffer_t* buffer,const e_skeleton_t* skeleton,const e_animation_t* anim);
/*!
  \brief Destroys a skeleton buffer
  \param[in] buffer The buffer to destroy
*/
void e_destroyskeletonbuffer(e_skeletonbuffer_t* buffer);

/*!
  \brief Loads a skeleton
  \param[in] device The device to use for loading
  \param[in] file The file to read data from
  \return 0 or a valid skeleton
*/
e_skeleton_t* e_loadskeleton(e_device_t* device,fs_handle_t file);
/*!
  \brief Destroys a skeleton
  \param[in] skeleton The skeleton to destroy
*/
void e_destroyskeleton(e_skeleton_t* skeleton);
/*!
  \brief Renders the given skeleton buffer (for debug use)
  \param[in] device The device to use
  \param[in] skeleton The skeleton to use for rendering
  \param[in] buffer The skeleton buffer touse for rendering
  \param[in] matrix The transformation to apply while rendering
*/
void e_renderskeleton(e_device_t* device,const e_skeleton_t* skeleton,const e_skeletonbuffer_t* buffer,const mt_matrix4_t matrix);
/*!
  \brief Gets the matrix of a bone
  \param[in,out] matrix The resulting matrix
  \param[in] bone The bone index
  \param[in] skeleton The skeleton object to use
  \param[in] buffer The skeleton buffer to use
*/
void e_getbonematrix(mt_matrix4_t matrix,unsigned int bone,const e_skeleton_t* skeleton,const e_skeletonbuffer_t* buffer);
/*!
  \brief Sets the next animation to play
  \param[in,out] anim The animation to update
  \param[in] skeleton The skeleton to use
  \param[in] action The next action to play
*/
void e_nextanimation(e_animation_t* anim,const e_skeleton_t* skeleton,unsigned int action);
/*!
  \brief Updates the given animation
  \param[in,out] anim The animation to update
  \param[in] skeleton The skeleton to use
  \param[in] step The frame time to add
  \param[in] lambda The blending factor to add
*/
void e_playanimation(e_animation_t* anim,const e_skeleton_t* skeleton,mt_scalar_t step,mt_scalar_t lambda);
/*!
  \brief Blocks the current animation to its end
  \param[in,out] anim The animation to update
  \param[in] skeleton The skeleton to use
  \param[in] step The frame time to add
  \param[in] lambda The blending factor to add
*/
void e_stopanimation(e_animation_t* anim,const e_skeleton_t* skeleton,mt_scalar_t step,mt_scalar_t lambda);

/*! @} */

/*! @} */

#endif
