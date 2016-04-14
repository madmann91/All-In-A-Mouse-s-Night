/***********************************************************
*                                                          *
*  File : e_model.h                                        *
*  Date : 13/11/2009                                       *
*  Comments : Model loading and rendering                  *
*                                                          *
************************************************************/

#ifndef __E_MODEL_H__
#define __E_MODEL_H__

#include "e_common.h"
#include "e_device.h"
#include "e_skeleton.h"
#include "e_texture.h"
#include "e_shadow.h"

#include "mt_vector.h"
#include "mt_plane.h"

#include "fs_common.h"

/*! \addtogroup e */
/*! @{ */

/*! \name Model */
/*! @{ */

#define E_MODEL_MAXBONES 4

extern const e_filetype_t e_modelfile;

#pragma pack(push,1)
struct e_modelvertex_s
{
	e_vertex3_t pos;
	e_vertex3_t normal;
	e_texcoord_t tex;
};
#pragma pack(pop)

typedef struct e_modelvertex_s e_modelvertex_t;

/*! Per vertex weight */
struct e_weight_s
{
	int bones[E_MODEL_MAXBONES];			/*!< The bones linked with the vertex */
	mt_scalar_t weights[E_MODEL_MAXBONES];	/*!< The corresponding weights */
};

typedef struct e_weight_s e_weight_t;

/*! Model flags */
enum E_MODEL_FLAGS
{
	E_MODEL_STATIC=0x01			/*!< This model is a static model (no weights) */
};

/*! A 3D model buffer */
struct e_modelbuffer_s
{
	unsigned int modelbuffer;	/*!< Model vertex buffer */
	unsigned int shadowbuffer;	/*!< Shadow vertex buffer */
	unsigned int* volume;		/*!< Shadow volume indices */
	unsigned int count;			/*!< The number of shadow volume indices */
	unsigned char* facing;		/*!< Face lighting flags */
	mt_plane_t* planes;			/*!< Plane equations */
};

typedef struct e_modelbuffer_s e_modelbuffer_t;

/*! The 3D model structure */
struct e_model_s
{
	int flags;

	unsigned int indexbuffer;		/*!< The index buffer id */
	unsigned int texcoordbuffer;	/*!< Texture coordinates buffer */
	mt_vector3_t* vertexbuffer;		/*!< Transformed vertices */

	e_modelvertex_t* vertices;		/*!< Untransformed vertices */
	unsigned int* indices;			/*!< Model indices */
	e_weight_t* weights;			/*!< The vertex weights of the model (one per each vertex) */
	e_edge_t* edges;				/*!< The model edges */

	unsigned int numedges;			/*!< The number of edges */
	unsigned int numvertices;		/*!< Number of vertices in the model */
	unsigned int numindices;		/*!< Number of indices in the model */

	union
	{
		mt_vector3_t halfextents;		/*!< The half dimensions of the enclosing box (for character models) */
		mt_vector3_t hotspot;			/*!< The position of the hotspot (for weapon models) */
	};
};

typedef struct e_model_s e_model_t;

/*!
  \brief Creates a model buffer
  \param[in] model The model to associate with the buffer
  \return A valid model buffer, or 0 if an error occured
*/
e_modelbuffer_t* e_createmodelbuffer(e_model_t* model);
/*!
  \brief Updates a model buffer
  \param[in] buffer The buffer to update
  \param[in] model The model associated with the buffer
  \param[in] skeleton The skeleton buffer to use for deformation
*/
void e_updatemodelbuffer(e_modelbuffer_t* buffer,const e_model_t* model,const e_skeletonbuffer_t* skeleton);
/*!
  \brief Builds a shadow volume
  \param[in] buffer The buffer to update
  \param[in] model The model associated with the buffer
  \param[in] light The light to use for shadow volume creation
*/
void e_buildshadowvolume(e_modelbuffer_t* buffer,const e_model_t* model,const mt_vector3_t light);
/*!
  \brief Destroys a model buffer
  \param[in] buffer The buffer to destroy
*/
void e_destroymodelbuffer(e_modelbuffer_t* buffer);

/*!
  \brief Loads a 3D model
  \param[in] device The device to use
  \param[in] file The file to read data from
  \return A valid model object, or 0 if an error occured
*/
e_model_t* e_loadmodel(e_device_t* device,fs_handle_t file);
/*!
  \brief Destroys a model
  \param[in] model The model to destroy
*/
void e_destroymodel(e_model_t* model);
/*!
  \brief Renders a model mesh
  \param[in] device The device to use
  \param[in] model The model to render
  \param[in] buffer The model buffer to use
  \param[in] texture The texture to use for skinning
  \param[in] gradient The gradient to use for shading
  \param[in] light The light position in model space
  \param[in] matrix The transformation to apply
  \param[in] shading The shading flag (set to 1 if you want shading, otherwise 0)
*/
void e_rendermodel(e_device_t* device,e_model_t* model,const e_modelbuffer_t* buffer,const e_texture_t* texture,const e_texture_t* gradient,const mt_vector3_t light,const mt_matrix4_t matrix,int shading);
/*!
  \brief Renders a model shadow
  \param[in] device The device to use
  \param[in] model The model to render
  \param[in] buffer The model buffer to use
  \param[in] light The light position in model space
  \param[in] matrix The transformation to apply
*/
void e_rendermodelshadow(e_device_t* device,e_model_t* model,const e_modelbuffer_t* buffer,const mt_vector3_t light,const mt_matrix4_t matrix);
/*!
  \brief Renders a model shadow
  \param[in] device The device to use
  \param[in] model The model to render
  \param[in] buffer The model buffer to use
  \param[in] matrix The transformation to apply
*/
void e_rendermodeloutlines(e_device_t* device,e_model_t* model,const e_modelbuffer_t* buffer,const mt_matrix4_t matrix);

/*! @} */

/*! @} */

#endif
