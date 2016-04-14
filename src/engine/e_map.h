/***********************************************************
*                                                          *
*  File : e_map.h                                          *
*  Date : 13/11/2009                                       *
*  Comments : Map loading and rendering                    *
*                                                          *
************************************************************/

#ifndef __E_MAP_H__
#define __E_MAP_H__

#include "e_common.h"
#include "e_device.h"
#include "e_texture.h"
#include "e_camera.h"

#include "fs_common.h"

/*! \addtogroup e */
/*! @{ */

/*! \name Map */
/*! @{ */

#define E_MATERIAL_MAXLIGHTS 4

extern const e_filetype_t e_mapfile;

#pragma pack(push,1)
struct e_mapvertex_s
{
	e_vertex3_t pos;
	e_vertex3_t norm;
	e_texcoord_t tex1;
};
#pragma pack(pop)

typedef struct e_mapvertex_s e_mapvertex_t;

enum E_MATERIAL_FLAGS
{
	E_MATERIAL_TEXTURE=0x01,
	E_MATERIAL_SHADING=0x02
};

/*! A map material */
struct e_mapmaterial_s
{
	int flags;							/*!< Material flags	(reserved for future evolution) */
	e_color4_t color;					/*!< The color of the material (RGBA format) */
	unsigned int texture;				/*!< The texture index of the material */
	unsigned int numlights;				/*!< Number of lights */
	unsigned int lights[E_MATERIAL_MAXLIGHTS];		/*!< The light indices */
};

typedef struct e_mapmaterial_s e_mapmaterial_t;

/*! A map cell */
struct e_mapcell_s
{
	char isleaf;				/*!< Leaf flag (1 if this cell is a leaf, otherwise 0) */
	unsigned int index;			/*!< The index in node/leaf array (depends on the value of isleaf) */
	unsigned int left,right;	/*!< The left and right cells index (only valid when the cell is not a leaf) */
};

typedef struct e_mapcell_s e_mapcell_t;

/*! A map node */
struct e_mapnode_s
{
	unsigned char axis;			/*!< Splitting axis */
	mt_vector3_t min;				/*!< Minimum bounding box coordinates */
	mt_vector3_t max;				/*!< Maximum bounding box coordinates */
	mt_scalar_t split;			/*!< Splitting point on axis */
};

typedef struct e_mapnode_s e_mapnode_t;

/*! A map leaf */
struct e_mapleaf_s
{
	unsigned int numpasses;		/*!< The number of rendering passes */
	unsigned int* passes;		/*!< An array of rendering passes */ 
	unsigned int numindices;	/*!< The number of indices */
	unsigned int* indices;		/*!< The indices of this leaf */
};

typedef struct e_mapleaf_s e_mapleaf_t;

/*! A map light */
struct e_maplight_s
{
	mt_vector3_t position;		/*!< The light position */
	e_color3_t color;			/*!< The light color */
};

typedef struct e_maplight_s e_maplight_t;

/*! Map structure */ 
struct e_map_s
{
	mt_vector3_t min;				/*!< The minimum bbox dimensions of the map */
	mt_vector3_t max;				/*!< The maximum bbox dimensions of the map */
	unsigned int vertexbuffer;		/*!< Vertex buffer object */
	unsigned int numvertices;		/*!< The number of map vertices */

	unsigned int numtextures;		/*!< Number of texture names */
	char** textures;				/*!< Texture names */

	e_mapmaterial_t* materials;		/*!< Material array */
	unsigned int nummaterials;		/*!< Number of materials */

	e_mapcell_t* cells;				/*!< The map cells */
	unsigned int numcells;			/*!< The number of cells */

	e_mapnode_t* nodes;				/*!< Map nodes array */
	unsigned int numnodes;			/*!< Number of nodes */

	e_mapleaf_t* leaves;			/*!< Map leaves */
	unsigned int numleaves;			/*!< Number of leaves */

	e_maplight_t* lights;			/*!< Map lights */
	unsigned int numlights;			/*!< Number of lights */

	unsigned int* geomindices;			/*!< Map geometry indices */
	unsigned int numgeomindices;		/*!< Number of geometry indices */
	mt_scalar_t* geomvertices;			/*!< Map geometry vertices */
	unsigned int numgeomvertices;		/*!< Number of geometry vertices */
};

typedef struct e_map_s e_map_t;

/*!
  \brief Loads a map
  \param[in] device The device to use
  \param[in] file The file to read data from
  \return A valid map object, or 0 if an error occured
*/
e_map_t* e_loadmap(e_device_t* device,fs_handle_t file);
/*!
  \brief Destroys a map
  \param[in] map The map to destroy
*/
void e_destroymap(e_map_t* map);
/*!
  \brief Renders a map
  \param[in] device The device to use
  \param[in] map The map to render
  \param[in] camera The camera containing clipping planes
  \param[in] textures The textures array
  \param[in] gradient The gradient to use for shading
  \param[in] light The light position in global space
  \param[in] shading The shading flag (set to 1 if you want shading, otherwise 0)
*/
void e_rendermap(e_device_t* device,const e_map_t* map,const e_camera_t* camera,const e_texture_t** textures,const e_texture_t* gradient,const mt_vector3_t light,int shading);

/*! @} */

/*! @} */

#endif